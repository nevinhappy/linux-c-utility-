#ifndef __UTI_CSTRING_H__
#define __UTI_CSTRING_H__

#include "platform.h"


typedef I8*  uti_cstring_t;
#define INVALID_UTI_STRING  NULL
//
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

uti_cstring_t cstring_create(U32 len);

uti_cstring_t cstring_create_s(I8* src);

void cstring_destory(uti_cstring_t str);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* __UTI_CSTRING_H__ */

