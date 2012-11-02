

#ifndef __UTI_TIME_H__
#define __UTI_TIME_H__


#include "platform.h"

//
#ifdef __cplusplus
extern "C" {
#endif //:__cplusplus


/*-------------------------------------------------------------------------*/
typedef struct _uti_time_s
{
	int year;			/* year */
	int month;			/* month */
	int day;			/* day of the month */
	int hour;			/* hours */
	int minute;			/* minutes */
	int second;			/* seconds */
	int microsecond;	/*microseconds*/
	int dayofweek;		/* day of the week */
	int dayofyear;		/* day in the year */
}UtiTime_t;

inline I64 uti_get_time_s();
inline void uti_get_time_t(UtiTime_t *time);

inline I64 uti_convert_time_s(const UtiTime_t *time);
inline void uti_convert_time_t(UtiTime_t *time, I64 value);

//
#ifdef __cplusplus
}
#endif //:__cplusplus

#endif //:__UTI_TIME_H__


