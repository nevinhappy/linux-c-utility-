
#include "uti_string.h"
#include "uti_mem_mgr.h"

static uti_int_t
ngx_decode_base64_internal(uti_str_t *dst, uti_str_t *src, const cstr_t basis);

/*****************************************************************************
 Prototype    : string_create
 Description  : memory allocation of string
 Input        : U32 len  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/15
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
cstr_t cstring_create(U32 len)
{
    cstr_t pstring = (cstr_t)PMEM_ALLOC(len*sizeof(I8));
    if(pstring) {
        memset(pstring, 0x00, len*sizeof(I8));
    }
    
    return pstring;
}

/*****************************************************************************
 Prototype    : string_create_s
 Description  : Create string object from a source string
 Input        : I8* src        
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/19
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
cstr_t cstring_create_s(I8* src)
{
    cstr_t pstring = cstring_create(strlen(src)+1);
    if(NULL == pstring) {
        return NULL;
    }
    strcpy((char*)pstring, src);
    return pstring;
}

void cstring_destory(cstr_t str)
{
    if(str) {
        PMEM_FREE(str);
    }
}

/****************************************************************************/
cstr_t ngx_hex_dump(cstr_t dst, cstr_t src, size_t len)
{
    static u_char  hex[] = "0123456789abcdef";

    while (len--) {
        *dst++ = hex[*src >> 4];
        *dst++ = hex[*src++ & 0xf];
    }

    return dst;
}

/**
 * DESC : convert hex format charactor to data, like 'a' -> 10
 * RETURN VALUE: -1 Error, otherwise, succeed.
 **/
u_char hex2ch(const u_char hex)
{
    /* checks for a hexadecimal digits */
    if(isxdigit(hex) == 0){
        /* error here */
        return -1;
    }

    if(hex>96) {
        return hex-87;
    } else if(hex>64) {
        return hex-55;
    } else {
        return hex-48;
    }
}

/** 
 * DESC: convert hex string to memory data and store to buf
 * RETURN VALUE: -1 ERROR, >0 then lenght of data to buf
 **/
int str2buf(const cstr_t str, int len, cstr_t buf)
{
    int index, indexj;

    char tmp;
    if((0 == len)||(len%2!=0)) return -1;
    
    for(index=0, indexj=0; index<len; index++) {
        tmp = hex2ch(str[index]);
        if(-1 == tmp) return -1;
        tmp = tmp<<4;
        tmp += hex2ch(str[++index]);
        buf[indexj++]=tmp;
    }
    return indexj;
}


void
ngx_encode_base64(uti_str_t *dst, uti_str_t *src)
{
    cstr_t          d, s;
    size_t          len;
    static u_char   basis64[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    len = src->len;
    s = src->str;
    d = dst->str;

    while (len > 2) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];
        *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
        *d++ = basis64[((s[1] & 0x0f) << 2) | (s[2] >> 6)];
        *d++ = basis64[s[2] & 0x3f];

        s += 3;
        len -= 3;
    }

    if (len) {
        *d++ = basis64[(s[0] >> 2) & 0x3f];

        if (len == 1) {
            *d++ = basis64[(s[0] & 3) << 4];
            *d++ = '=';

        } else {
            *d++ = basis64[((s[0] & 3) << 4) | (s[1] >> 4)];
            *d++ = basis64[(s[1] & 0x0f) << 2];
        }

        *d++ = '=';
    }

    dst->len = d - dst->str;
}


uti_int_t
ngx_decode_base64(uti_str_t *dst, uti_str_t *src)
{
    static u_char   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77, 77, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 77,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return ngx_decode_base64_internal(dst, src, basis64);
}


uti_int_t
ngx_decode_base64url(uti_str_t *dst, uti_str_t *src)
{
    static u_char   basis64[] = {
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 62, 77, 77,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 77, 77, 77, 77, 77,
        77,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 77, 77, 77, 77, 63,
        77, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 77, 77, 77, 77, 77,

        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    return ngx_decode_base64_internal(dst, src, basis64);
}


static uti_int_t
ngx_decode_base64_internal(uti_str_t *dst, uti_str_t *src, const cstr_t basis)
{
    size_t          len;
    cstr_t          d, s;

    for (len = 0; len < src->len; len++) {
        if (src->str[len] == '=') {
            break;
        }

        if (basis[src->str[len]] == 77) {
            return UTI_ERROR;
        }
    }

    if (len % 4 == 1) {
        return UTI_ERROR;
    }

    s = src->str;
    d = dst->str;

    while (len > 3) {
        *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
        *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
        *d++ = (u_char) (basis[s[2]] << 6 | basis[s[3]]);

        s += 4;
        len -= 4;
    }

    if (len > 1) {
        *d++ = (u_char) (basis[s[0]] << 2 | basis[s[1]] >> 4);
    }

    if (len > 2) {
        *d++ = (u_char) (basis[s[1]] << 4 | basis[s[2]] >> 2);
    }

    dst->len = d - dst->str;

    return UTI_OK;
}


/*
 * ngx_utf8_decode() decodes two and more bytes UTF sequences only
 * the return values:
 *    0x80 - 0x10ffff         valid character
 *    0x110000 - 0xfffffffd   invalid sequence
 *    0xfffffffe              incomplete sequence
 *    0xffffffff              error
 */

uint32_t
ngx_utf8_decode(cstr_t *p, size_t n)
{
    size_t    len;
    uint32_t  u, i, valid;

    u = **p;

    if (u >= 0xf0) {

        u &= 0x07;
        valid = 0xffff;
        len = 3;

    } else if (u >= 0xe0) {

        u &= 0x0f;
        valid = 0x7ff;
        len = 2;

    } else if (u >= 0xc2) {

        u &= 0x1f;
        valid = 0x7f;
        len = 1;

    } else {
        (*p)++;
        return 0xffffffff;
    }

    if (n - 1 < len) {
        return 0xfffffffe;
    }

    (*p)++;

    while (len) {
        i = *(*p)++;

        if (i < 0x80) {
            return 0xffffffff;
        }

        u = (u << 6) | (i & 0x3f);

        len--;
    }

    if (u > valid) {
        return u;
    }

    return 0xffffffff;
}


size_t
ngx_utf8_length(cstr_t p, size_t n)
{
    u_char  c;
    cstr_t  last;
    size_t  len;

    last = p + n;

    for (len = 0; p < last; len++) {

        c = *p;

        if (c < 0x80) {
            p++;
            continue;
        }

        if (ngx_utf8_decode(&p, n) > 0x10ffff) {
            /* invalid UTF-8 */
            return n;
        }
    }

    return len;
}


cstr_t ngx_utf8_cpystrn(cstr_t dst, cstr_t src, size_t n, size_t len)
{
    u_char  c;
    cstr_t  next;

    if (n == 0) {
        return dst;
    }

    while (--n) {

        c = *src;
        *dst = c;

        if (c < 0x80) {

            if (c != '\0') {
                dst++;
                src++;
                len--;

                continue;
            }

            return dst;
        }

        next = src;

        if (ngx_utf8_decode(&next, len) > 0x10ffff) {
            /* invalid UTF-8 */
            break;
        }

        while (src < next) {
            *dst++ = *src++;
            len--;
        }
    }

    *dst = '\0';

    return dst;
}


uintptr_t
ngx_escape_uri(cstr_t dst, cstr_t src, size_t size, uti_int_t type)
{
    uti_int_t      n;
    uint32_t       *escape;
    static u_char   hex[] = "0123456789abcdef";

                    /* " ", "#", "%", "?", %00-%1F, %7F-%FF */

    static uint32_t   uri[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x80000029, /* 1000 0000 0000 0000  0000 0000 0010 1001 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", "#", "%", "&", "+", "?", %00-%1F, %7F-%FF */

    static uint32_t   args[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x88000869, /* 1000 1000 0000 0000  0000 1000 0110 1001 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* not ALPHA, DIGIT, "-", ".", "_", "~" */

    static uint32_t   uri_component[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0xfc009fff, /* 1111 1100 0000 0000  1001 1111 1111 1111 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x78000001, /* 0111 1000 0000 0000  0000 0000 0000 0001 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0xb8000001, /* 1011 1000 0000 0000  0000 0000 0000 0001 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", "#", """, "%", "'", %00-%1F, %7F-%FF */

    static uint32_t   html[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x000000ad, /* 0000 0000 0000 0000  0000 0000 1010 1101 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", """, "%", "'", %00-%1F, %7F-%FF */

    static uint32_t   refresh[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x00000085, /* 0000 0000 0000 0000  0000 0000 1000 0101 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000  0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */
        0xffffffff  /* 1111 1111 1111 1111  1111 1111 1111 1111 */
    };

                    /* " ", "%", %00-%1F */

    static uint32_t   memcached[] = {
        0xffffffff, /* 1111 1111 1111 1111  1111 1111 1111 1111 */

                    /* ?>=< ;:98 7654 3210  /.-, +*)( '&%$ #"!  */
        0x00000021, /* 0000 0000 0000 0000  0000 0000 0010 0001 */

                    /* _^]\ [ZYX WVUT SRQP  ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

                    /*  ~}| {zyx wvut srqp  onml kjih gfed cba` */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */

        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000  0000 0000 0000 0000 */
    };

                    /* mail_auth is the same as memcached */

    static uint32_t  *map[] =
        { uri, args, uri_component, html, refresh, memcached, memcached };


    escape = map[type];

    if (dst == NULL) {

        /* find the number of the characters to be escaped */

        n = 0;

        while (size) {
            if (escape[*src >> 5] & (1 << (*src & 0x1f))) {
                n++;
            }
            src++;
            size--;
        }

        return (uintptr_t) n;
    }

    while (size) {
        if (escape[*src >> 5] & (1 << (*src & 0x1f))) {
            *dst++ = '%';
            *dst++ = hex[*src >> 4];
            *dst++ = hex[*src & 0xf];
            src++;

        } else {
            *dst++ = *src++;
        }
        size--;
    }

    return (uintptr_t) dst;
}


void
ngx_unescape_uri(cstr_t *dst, cstr_t *src, size_t size, uti_int_t type)
{
    cstr_t  d, s;
    u_char  ch, c, decoded;
    enum {
        sw_usual = 0,
        sw_quoted,
        sw_quoted_second
    } state;

    d = *dst;
    s = *src;

    state = 0;
    decoded = 0;

    while (size--) {

        ch = *s++;

        switch (state) {
        case sw_usual:
            if (ch == '?'
                && (type & (NGX_UNESCAPE_URI|NGX_UNESCAPE_REDIRECT)))
            {
                *d++ = ch;
                goto done;
            }

            if (ch == '%') {
                state = sw_quoted;
                break;
            }

            *d++ = ch;
            break;

        case sw_quoted:

            if (ch >= '0' && ch <= '9') {
                decoded = (u_char) (ch - '0');
                state = sw_quoted_second;
                break;
            }

            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                decoded = (u_char) (c - 'a' + 10);
                state = sw_quoted_second;
                break;
            }

            /* the invalid quoted character */

            state = sw_usual;

            *d++ = ch;

            break;

        case sw_quoted_second:

            state = sw_usual;

            if (ch >= '0' && ch <= '9') {
                ch = (u_char) ((decoded << 4) + ch - '0');

                if (type & NGX_UNESCAPE_REDIRECT) {
                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }

                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);

                    break;
                }

                *d++ = ch;

                break;
            }

            c = (u_char) (ch | 0x20);
            if (c >= 'a' && c <= 'f') {
                ch = (u_char) ((decoded << 4) + c - 'a' + 10);

                if (type & NGX_UNESCAPE_URI) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }

                    *d++ = ch;
                    break;
                }

                if (type & NGX_UNESCAPE_REDIRECT) {
                    if (ch == '?') {
                        *d++ = ch;
                        goto done;
                    }

                    if (ch > '%' && ch < 0x7f) {
                        *d++ = ch;
                        break;
                    }

                    *d++ = '%'; *d++ = *(s - 2); *d++ = *(s - 1);
                    break;
                }

                *d++ = ch;

                break;
            }

            /* the invalid quoted character */

            break;
        }
    }

done:

    *dst = d;
    *src = s;
}


uintptr_t
ngx_escape_html(cstr_t dst, cstr_t src, size_t size)
{
    u_char      ch;
    uti_int_t  len;

    if (dst == NULL) {

        len = 0;

        while (size) {
            switch (*src++) {

            case '<':
                len += sizeof("&lt;") - 2;
                break;

            case '>':
                len += sizeof("&gt;") - 2;
                break;

            case '&':
                len += sizeof("&amp;") - 2;
                break;

            case '"':
                len += sizeof("&quot;") - 2;
                break;

            default:
                break;
            }
            size--;
        }

        return (uintptr_t) len;
    }

    while (size) {
        ch = *src++;

        switch (ch) {

        case '<':
            *dst++ = '&'; *dst++ = 'l'; *dst++ = 't'; *dst++ = ';';
            break;

        case '>':
            *dst++ = '&'; *dst++ = 'g'; *dst++ = 't'; *dst++ = ';';
            break;

        case '&':
            *dst++ = '&'; *dst++ = 'a'; *dst++ = 'm'; *dst++ = 'p';
            *dst++ = ';';
            break;

        case '"':
            *dst++ = '&'; *dst++ = 'q'; *dst++ = 'u'; *dst++ = 'o';
            *dst++ = 't'; *dst++ = ';';
            break;

        default:
            *dst++ = ch;
            break;
        }
        size--;
    }

    return (uintptr_t) dst;
}


