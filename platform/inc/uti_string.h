#ifndef __UTI_CSTRING_H__
#define __UTI_CSTRING_H__

#include <ctype.h>

#include "platform.h"

/**
 * cstr_t : redefine of c string 
 **/
typedef u_char*  cstr_t;
#define NULL_CSTR   NULL

/**
 * uti_str_t : 
 **/
typedef struct {
    size_t      len;
    cstr_t      str;
} uti_str_t;

//
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*********************************************************************
 * cstr_t operations 
 **/
cstr_t cstr_create(U32 len);

cstr_t cstring_create_s(I8* src);

void cstring_destory(cstr_t str);

/*********************************************************************
 * cstr_t operations 
 **/

#define NGX_ESCAPE_URI            0
#define NGX_ESCAPE_ARGS           1
#define NGX_ESCAPE_URI_COMPONENT  2
#define NGX_ESCAPE_HTML           3
#define NGX_ESCAPE_REFRESH        4
#define NGX_ESCAPE_MEMCACHED      5
#define NGX_ESCAPE_MAIL_AUTH      6

#define NGX_UNESCAPE_URI       1
#define NGX_UNESCAPE_REDIRECT  2

#define uti_tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)
#define uti_toupper(c)      (u_char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)

#define uti_string(str)     { sizeof(str) - 1, (cstr_t) str }
#define uti_null_string     { 0, NULL }
#define uti_str_set(str, text)   \
    (str)->len = sizeof(text) - 1; (str)->data = (cstr_t) text
#define ngx_str_null(str)   (str)->len = 0; (str)->data = NULL




#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* __UTI_CSTRING_H__ */

