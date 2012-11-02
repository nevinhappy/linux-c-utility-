#ifndef __UTI_BUFF_H__
#define __UTI_BUFF_H__

#include "platform.h"
#include "uti_file.h"

typedef struct uti_chain_s uti_chain_t;

typedef void *            uti_buf_tag_t;

typedef struct uti_buf_s  uti_buf_t;

struct uti_buf_s {
    u_char          *pos;
    u_char          *last;
    off_t            file_pos;
    off_t            file_last;

    u_char          *start;         /* start of buffer */
    u_char          *end;           /* end of buffer */
    uti_buf_tag_t    tag;
    file_desc_t     *file;
    uti_buf_t       *shadow;


    /* the buf's content could be changed */
    unsigned         temporary:1;

    /*
     * the buf's content is in a memory cache or in a read only memory
     * and must not be changed
     */
    unsigned         memory:1;

    /* the buf's content is mmap()ed and must not be changed */
    unsigned         mmap:1;

    unsigned         recycled:1;
    unsigned         in_file:1;
    unsigned         flush:1;
    unsigned         sync:1;
    unsigned         last_buf:1;
    unsigned         last_in_chain:1;

    unsigned         last_shadow:1;
    unsigned         temp_file:1;

    /* STUB */ int   num;
};


struct uti_chain_s {
    uti_buf_t    *buf;
    uti_chain_t  *next;
};

#endif /*__UTI_BUFF_H__*/


