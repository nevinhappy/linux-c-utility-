// plantform.c : provide os system call functions 
//
//////////////////////////////////////////////////////////////////////

#include "platform.h"


////////////////////////////////////////////////////////////////////////////////
//	platform independent functions

#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <time.h>
#include <sys/time.h>

/*-----------------------------------------------------------------------------*/
const char* os_get_env(const char *name)
{
	return (const char*)getenv(name);
}

BOOL os_set_env(const char *name, const char *value)
{
	return 0 == setenv(name, value, TRUE);
}

BOOL os_remove_env(const char *name)
{
	return 0 == unsetenv(name);
}

U32 os_random(U32 range)
{
	static BOOL s_bInited = FALSE;
	if (! s_bInited)
	{
		srand((int)time(NULL));
		s_bInited = TRUE;
	}
	if (range != 0)
		return (U32)((rand()/(RAND_MAX + 1.0))*range);
	return (U32)rand();
}

BOOL os_sleep(U32 nMS)
{
	usleep(nMS * 1000);
	return TRUE;
}

void os_set_signal_handler(int sig, psignal_proc_t on_signal_f)
{
	signal(sig, on_signal_f);
}


