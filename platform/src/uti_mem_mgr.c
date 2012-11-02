/************************************************************************
 * Memory Pool logic is referred from Nginx code. 
 *
 ***********************************************************************/

#include <malloc.h>

#include "uti_mem_mgr.h"
#include "uti_logging.h"
#include "uti_errno.h"

uti_uint_t  uti_pagesize;

static void *uti_palloc_block(uti_pool_t *pool, size_t size);
static void *uti_palloc_large(uti_pool_t *pool, size_t size);

/**
 * init page size
 * NOTE:this function should be called before any others
 **/
void mem_init()
{
    uti_pagesize = getpagesize();
}


void *
uti_alloc(size_t size)
{
    void  *p;

    p = PMEM_ALLOC(size);
    if (p == NULL) {
        LOGGING_ERROR("malloc(%uz) failed", size);
    }

    LOGGING_DEBUG("malloc: %p:%uz", p, size);

    return p;
}


void *
uti_calloc(size_t size)
{
    void  *p;

    p = uti_alloc(size);

    if (p) {
        uti_memzero(p, size);
    }

    return p;
}


#if (UTI_HAVE_POSIX_MEMALIGN)

void *
uti_memalign(size_t alignment, size_t size)
{
    void  *p;
    int    err;

    err = posix_memalign(&p, alignment, size);

    if (err) {
        LOGGING_ERROR("posix_memalign(%uz, %uz) failed", alignment, size);
        p = NULL;
    }

    LOGGING_DEBUG("posix_memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

#elif (UTI_HAVE_MEMALIGN)

void *
uti_memalign(size_t alignment, size_t size)
{
    void  *p;

    p = memalign(alignment, size);
    if (p == NULL) {
        LOGGING_ERROR("memalign(%uz, %uz) failed", alignment, size);
    }

    LOGGING_DEBUG("memalign: %p:%uz @%uz", p, size, alignment);

    return p;
}

#endif /*UTI_HAVE_POSIX_MEMALIGN*/

uti_pool_t *
uti_create_pool(size_t size)
{
    uti_pool_t  *p;

    p = uti_memalign(UTI_POOL_ALIGNMENT, size);
    if (p == NULL) {
        return NULL;
    }

    p->d.last = (u_char *) p + sizeof(uti_pool_t);
    p->d.end = (u_char *) p + size;
    p->d.next = NULL;
    p->d.failed = 0;

    size = size - sizeof(uti_pool_t);
    p->max = (size < UTI_MAX_ALLOC_FROM_POOL) ? size : UTI_MAX_ALLOC_FROM_POOL;

    p->current = p;
    p->chain = NULL;
    p->large = NULL;
    p->cleanup = NULL;

    return p;
}


void
uti_destroy_pool(uti_pool_t *pool)
{
    uti_pool_t          *p, *n;
    uti_pool_large_t    *l;
    uti_pool_cleanup_t  *c;

    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            LOGGING_DEBUG("run cleanup: %p", c);
            c->handler(c->data);
        }
    }

    for (l = pool->large; l; l = l->next) {

        LOGGING_DEBUG("free: %p", l->alloc);

        if (l->alloc) {
            PMEM_FREE(l->alloc);
        }
    }

#if (UTI_DEBUG)

    /*
     * we could allocate the pool->log from this pool
     * so we cannot use this log while free()ing the pool
     */

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        LOGGING_DEBUG("free: %p, unused: %uz", p, p->d.end - p->d.last);

        if (n == NULL) {
            break;
        }
    }

#endif

    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
        PMEM_FREE(p);

        if (n == NULL) {
            break;
        }
    }
}


void
uti_reset_pool(uti_pool_t *pool)
{
    uti_pool_t        *p;
    uti_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            PMEM_FREE(l->alloc);
        }
    }

    pool->large = NULL;

    for (p = pool; p; p = p->d.next) {
        p->d.last = (u_char *) p + sizeof(uti_pool_t);
    }
}


void *
uti_palloc(uti_pool_t *pool, size_t size)
{
    u_char      *m;
    uti_pool_t  *p;

    if (size <= pool->max) {

        p = pool->current;

        do {
            m = uti_align_ptr(p->d.last, UTI_ALIGNMENT);

            if ((size_t) (p->d.end - m) >= size) {
                p->d.last = m + size;

                return m;
            }

            p = p->d.next;

        } while (p);

        return uti_palloc_block(pool, size);
    }

    return uti_palloc_large(pool, size);
}


void *
uti_pnalloc(uti_pool_t *pool, size_t size)
{
    u_char      *m;
    uti_pool_t  *p;

    if (size <= pool->max) {

        p = pool->current;

        do {
            m = p->d.last;

            if ((size_t) (p->d.end - m) >= size) {
                p->d.last = m + size;

                return m;
            }

            p = p->d.next;

        } while (p);

        return uti_palloc_block(pool, size);
    }

    return uti_palloc_large(pool, size);
}


static void *
uti_palloc_block(uti_pool_t *pool, size_t size)
{
    u_char      *m;
    size_t       psize;
    uti_pool_t  *p, *new, *current;

    psize = (size_t) (pool->d.end - (u_char *) pool);

    m = uti_memalign(UTI_POOL_ALIGNMENT, psize);
    if (m == NULL) {
        return NULL;
    }

    new = (uti_pool_t *) m;

    new->d.end = m + psize;
    new->d.next = NULL;
    new->d.failed = 0;

    m += sizeof(uti_pool_data_t);
    m = uti_align_ptr(m, UTI_ALIGNMENT);
    new->d.last = m + size;

    current = pool->current;

    for (p = current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            current = p->d.next;
        }
    }

    p->d.next = new;

    pool->current = current ? current : new;

    return m;
}


static void *
uti_palloc_large(uti_pool_t *pool, size_t size)
{
    void              *p;
    uti_uint_t         n;
    uti_pool_large_t  *large;

    p = uti_alloc(size);
    if (p == NULL) {
        return NULL;
    }

    n = 0;

    for (large = pool->large; large; large = large->next) {
        if (large->alloc == NULL) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }

    large = uti_palloc(pool, sizeof(uti_pool_large_t));
    if (large == NULL) {
        PMEM_FREE(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


void *
uti_pmemalign(uti_pool_t *pool, size_t size, size_t alignment)
{
    void              *p;
    uti_pool_large_t  *large;

    p = uti_memalign(alignment, size);
    if (p == NULL) {
        return NULL;
    }

    large = uti_palloc(pool, sizeof(uti_pool_large_t));
    if (large == NULL) {
        PMEM_FREE(p);
        return NULL;
    }

    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}


uti_int_t
uti_pfree(uti_pool_t *pool, void *p)
{
    uti_pool_large_t  *l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            LOGGING_DEBUG("free: %p", l->alloc);
            PMEM_FREE(l->alloc);
            l->alloc = NULL;

            return UTI_OK;
        }
    }

    return UTI_DECLINED;
}


void *
uti_pcalloc(uti_pool_t *pool, size_t size)
{
    void *p;

    p = uti_palloc(pool, size);
    if (p) {
        uti_memzero(p, size);
    }

    return p;
}


uti_pool_cleanup_t *
uti_pool_cleanup_add(uti_pool_t *p, size_t size)
{
    uti_pool_cleanup_t  *c;

    c = uti_palloc(p, sizeof(uti_pool_cleanup_t));
    if (c == NULL) {
        return NULL;
    }

    if (size) {
        c->data = uti_palloc(p, size);
        if (c->data == NULL) {
            return NULL;
        }

    } else {
        c->data = NULL;
    }

    c->handler = NULL;
    c->next = p->cleanup;

    p->cleanup = c;

    LOGGING_DEBUG("add cleanup: %p", c);

    return c;
}


void
uti_pool_run_cleanup_file(uti_pool_t *p, file_desc_t fd)
{
    uti_pool_cleanup_t       *c;
    uti_pool_cleanup_file_t  *cf;

    for (c = p->cleanup; c; c = c->next) {
        if (c->handler == uti_pool_cleanup_file) {

            cf = c->data;

            if (cf->fd == fd) {
                c->handler(cf);
                c->handler = NULL;
                return;
            }
        }
    }
}


void
uti_pool_cleanup_file(void *data)
{
    uti_pool_cleanup_file_t  *c = data;

    LOGGING_DEBUG("file cleanup: fd:%d", c->fd);

    if (os_close_file(c->fd) == FILE_OP_ERROR) {
        LOGGING_ERROR(" \"%s\" failed", c->name);
    }
}


void
uti_pool_delete_file(void *data)
{
    uti_pool_cleanup_file_t  *c = data;

    uti_err_t  err;

    LOGGING_DEBUG("file cleanup: fd:%d %s", c->fd, c->name);

    if (os_rm_file(c->name) == FILE_OP_ERROR) {
        err = uti_errno;

        if (err != UTI_ENOENT) {
            LOGGING_ERROR(" \"%s\" failed", c->name);
        }
    }

    if (os_close_file(c->fd) == FILE_OP_ERROR) {
        LOGGING_ERROR(" \"%s\" failed", c->name);
    }
}



