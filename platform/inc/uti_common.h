/******************************************************************************

  Copyright (C), 2010-2012, 

 ******************************************************************************
  File Name     : uti_common.h
  Version       : Initial Draft
  Author        : xiongjunyong
  Created       : 2012/6/4
  Last Modified :
  Description   : common utility define
  Function List :
  History       :
  1.Date        : 2012/6/4
    Author      : xiongjunyong
    Modification: Created file

******************************************************************************/

#ifndef __UTI_COMMON_H__
#define __UTI_COMMON_H__

#include "platform.h"

//
#ifdef __cplusplus
extern "C" {
#endif

/*
 * define structure to store ipv4 address
 */
typedef union _ipv4_addr_s {
    U32 value;
    struct {
        U8 addr1;
        U8 addr2;
        U8 addr3;
        U8 addr4;
    };
} ipv4_addr_t;



//
#ifdef __cplusplus
}
#endif


#endif /* __UTI_COMMON_H__ */

