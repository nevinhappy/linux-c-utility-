#ifndef __UTI_THREAD_H__
#define __UTI_THREAD_H__


#include "platform.h"

//
#ifdef __cplusplus
extern "C" {
#endif //:__cplusplus

#include <pthread.h>
#include <semaphore.h>


/****************************************************************/
typedef pthread_t   thread_desc_t;
typedef void* (*FThreadProc)(void*);
#define INVALID_THREAD_DESC         -1

typedef enum E_THREAD_SCHEDULE_POLICY
{
	tspRegular		= SCHED_OTHER,	//no priority
	tspRoundRobin	= SCHED_RR,		//priority between 1 ~ 99, greater means higher priority
	tspFIFO			= SCHED_FIFO	//priority between 1 ~ 99, greater means higher priority
} EThreadSchedPolicy;

BOOL thread_create( thread_desc_t *pthread, FThreadProc fProc, void *pParam, EThreadSchedPolicy nPolicy, int nPriority);
BOOL thread_join(thread_desc_t *pthread, void **ppResult);
thread_desc_t thread_id();

/**************************************************************
 * MUTEX LOCK 
 *************************************************************/
typedef pthread_mutex_t thread_lock_t;

void thread_lock_init(thread_lock_t *thread_lock, BOOL bRecursive);
BOOL thread_lock_lock(thread_lock_t *thread_lock);
BOOL thread_lock_try_lock(thread_lock_t *thread_lock);
BOOL thread_lock_unlock(thread_lock_t *thread_lock);
void thread_lock_destory(thread_lock_t *thread_lock);

/****************************************************************/

/**************************************************************
 * RW LOCK 
 *************************************************************/
typedef pthread_rwlock_t thread_rwlock_t;
#define PREFER_READER               PTHREAD_RWLOCK_PREFER_READER_NP
#define PREFER_WRITER               PTHREAD_RWLOCK_PREFER_WRITER_NP
#define PREFER_WRITER_NONRECURSIVE  PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP
BOOL thread_rwlock_init(thread_rwlock_t *thread_rwlock, U32 type);
BOOL thread_rwlock_rdlock(thread_rwlock_t *thread_rwlock);
BOOL thread_rwlock_try_rdlock(thread_rwlock_t *thread_rwlock);
BOOL thread_rwlock_wrlock(thread_rwlock_t *thread_rwlock);
BOOL thread_rwlock_try_wrlock(thread_rwlock_t *thread_rwlock);
BOOL thread_rwlock_unlock(thread_rwlock_t *thread_rwlock);
BOOL thread_rwlock_destory(thread_rwlock_t *thread_rwlock);

/****************************************************************/

//
#ifdef __cplusplus
}
#endif //:__cplusplus

#endif //:__UTI_THREAD_H__
