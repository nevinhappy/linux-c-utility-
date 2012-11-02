/************************************************************************
 * Memory Pool logic is referred from Nginx code. 
 *
 ***********************************************************************/
#ifndef __UTI_MEM_MGR_H__
#define __UTI_MEM_MGR_H__

#include "platform.h"
#include "uti_file.h"
#include "uti_buff.h"
//
#ifdef __cplusplus
extern "C" {
#endif //:__cplusplus

#define PMEM_ALLOC(size)     malloc(size)
#define PMEM_FREE(ptr)         do{ if(ptr)   \
                                 free(ptr);  \
                             }while(0);
/*************************************************************************/

#ifndef UTI_ALIGNMENT
#define UTI_ALIGNMENT   sizeof(unsigned long)    /* platform word */
#endif

#define  UTI_OK          0
#define  UTI_ERROR      -1
#define  UTI_AGAIN      -2
#define  UTI_BUSY       -3
#define  UTI_DONE       -4
#define  UTI_DECLINED   -5
#define  UTI_ABORT      -6

/*************************************************************************/
#define uti_memzero(buf, n)       (void) memset(buf, 0, n)
#define uti_memset(buf, c, n)     (void) memset(buf, c, n)

#define uti_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define uti_align_ptr(p, a)     \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

/*************************************************************************/

typedef intptr_t        uti_int_t;
typedef uintptr_t       uti_uint_t;

typedef struct uti_pool_s        uti_pool_t;


extern uti_uint_t  uti_pagesize;

/*
 * UTI_MAX_ALLOC_FROM_POOL should be (uti_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define UTI_MAX_ALLOC_FROM_POOL  (uti_pagesize - 1)

#define UTI_DEFAULT_POOL_SIZE    (16 * 1024)

#define UTI_POOL_ALIGNMENT       16
#define UTI_MIN_POOL_SIZE                                            \
    uti_align((sizeof(uti_pool_t) + 2 * sizeof(uti_pool_large_t)),  \
              UTI_POOL_ALIGNMENT)

/*************************************************************************/
typedef void (*uti_pool_cleanup_pt)(void *data);

typedef struct uti_pool_cleanup_s  uti_pool_cleanup_t;

struct uti_pool_cleanup_s {
    uti_pool_cleanup_pt   handler;
    void                 *data;
    uti_pool_cleanup_t   *next;
};


typedef struct uti_pool_large_s  uti_pool_large_t;

struct uti_pool_large_s {
    uti_pool_large_t     *next;
    void                 *alloc;
};


typedef struct {
    u_char               *last;
    u_char               *end;
    uti_pool_t           *next;
    uti_uint_t            failed;
} uti_pool_data_t;


struct uti_pool_s {
    uti_pool_data_t       d;
    size_t                max;
    uti_pool_t           *current;
    uti_chain_t          *chain;
    uti_pool_large_t     *large;
    uti_pool_cleanup_t   *cleanup;
};


typedef struct {
    file_desc_t          fd;
    char                *name;
} uti_pool_cleanup_file_t;

/******************************************************************************/

#if (UTI_HAVE_POSIX_MEMALIGN || UTI_HAVE_MEMALIGN)

void *uti_memalign(size_t alignment, size_t size);

#else

#define uti_memalign(alignment, size)  uti_alloc(size)

#endif


void mem_init();

void *uti_alloc(size_t size);
void *uti_calloc(size_t size);

uti_pool_t *uti_create_pool(size_t size);
void uti_destroy_pool(uti_pool_t *pool);
void uti_reset_pool(uti_pool_t *pool);

void *uti_palloc(uti_pool_t *pool, size_t size);
void *uti_pnalloc(uti_pool_t *pool, size_t size);
void *uti_pcalloc(uti_pool_t *pool, size_t size);
void *uti_pmemalign(uti_pool_t *pool, size_t size, size_t alignment);
uti_int_t uti_pfree(uti_pool_t *pool, void *p);


uti_pool_cleanup_t *uti_pool_cleanup_add(uti_pool_t *p, size_t size);
void uti_pool_run_cleanup_file(uti_pool_t *p, file_desc_t fd);
void uti_pool_cleanup_file(void *data);
void uti_pool_delete_file(void *data);



//
#ifdef __cplusplus
}
#endif //:__cplusplus

#endif /*__UTI_MEM_MGR_H__*/

