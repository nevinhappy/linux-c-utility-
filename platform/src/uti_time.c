/******************************************************************************

  Copyright (C), 2010-2012, 

 ******************************************************************************
  File Name     : uti_time.c
  Version       : Initial Draft
  Author        : xiongjunyong
  Created       : 
  Last Modified :
  Description   : time utility implementation
  Function List :
              uti_convert_time_s
              uti_convert_time_t
              uti_get_time_s
              uti_get_time_t
  History       :
  1.Date        : 2012/6/6
    Author      : xiongjunyong
    Modification: Created file

******************************************************************************/
#include <time.h>
#include <sys/time.h>

#include "uti_time.h"

//convert struct tm to struct UtiTime_t
#define COPY_SYS_2_TU(t, s)	\
	if (1) {(t).year=(s).tm_year+1900;(t).month=(s).tm_mon+1;(t).day=(s).tm_mday;	\
		(t).hour=(s).tm_hour;(t).minute=(s).tm_min;(t).second=(s).tm_sec;	\
		(t).microsecond=0;(t).dayofweek=(s).tm_wday;(t).dayofyear=(s).tm_yday;}

//convert struct UtiTime_t to struct tm
#define COPY_TU_2_SYS(s, t)	\
	if (1) {(s).tm_year=(t).year-1900;(s).tm_mon=(t).month-1;(s).tm_mday=(t).day;	\
		(s).tm_hour=(t).hour;(s).tm_min=(t).minute;(s).tm_sec=(t).second;	\
		(s).tm_wday=(t).dayofweek;(s).tm_yday=(t).dayofyear;}

/*****************************************************************************
 Prototype    : uti_get_time_s
 Description  : get current time, ms
 Input        : None
 Output       : None
 Return Value : microsecond
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/6
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
inline I64 uti_get_time_s()
{
	struct timezone tz;
	struct timeval tv;
	gettimeofday(&tv, &tz);
	return ((I64)tv.tv_sec - tz.tz_minuteswest * 60) * 1000000 + tv.tv_usec;
}

/*****************************************************************************
 Prototype    : uti_get_time_t
 Description  : get current time and store in struct UtiTime_t
 Input        : UtiTime_t *t  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/6
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
inline void uti_get_time_t(UtiTime_t *t)
{
	struct timeval tv;
	struct tm ts;
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &ts);
	COPY_SYS_2_TU((*t), ts);
	(*t).microsecond = tv.tv_usec;
}

/*****************************************************************************
 Prototype    : uti_convert_time_s
 Description  : time format convert 
 Input        : const UtiTime_t *t  
 Output       : None
 Return Value : microsecond
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/6
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
inline I64 uti_convert_time_s(const UtiTime_t *t)
{
	struct timezone tz;
	time_t tt;
	struct tm ts;
	gettimeofday(NULL, &tz);
	COPY_TU_2_SYS(ts, (*t));
	tt = mktime(&ts);
	return ((I64)tt - tz.tz_minuteswest * 60) * 1000000 + (*t).microsecond;
}

/*****************************************************************************
 Prototype    : uti_convert_time_t
 Description  : ime format convert
 Input        : UtiTime_t *t  
                I64 v         
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/6
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
inline void uti_convert_time_t(UtiTime_t *t, I64 v)
{
	time_t tt;
	struct tm ts;
	tt = (time_t)(v / 1000000);
	gmtime_r(&tt, &ts);
	COPY_SYS_2_TU((*t), ts);
	(*t).microsecond = (suseconds_t)(v - ((I64)tt) * 1000000);
}


