
#include "uti_cstring.h"
#include "uti_mem_mgr.h"


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
uti_cstring_t cstring_create(U32 len)
{
    uti_cstring_t pstring = (uti_cstring_t)PMEM_ALLOC(len*sizeof(I8));
    if(pstring)
    {
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
uti_cstring_t cstring_create_s(I8* src)
{
    uti_cstring_t pstring = cstring_create(strlen(src)+1);
    if(NULL == pstring)
    {
        return NULL;
    }
    strcpy(pstring, src);
    return pstring;
}

void cstring_destory(uti_cstring_t str)
{
    if(str)
    {
        PMEM_FREE(str);
    }
}

