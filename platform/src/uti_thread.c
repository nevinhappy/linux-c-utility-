/*-------------------------------------------------------------------------*/
// uti_thread.c :

#include "uti_thread.h"

/*------------------------------------------------------------------------------------*/
BOOL thread_create(thread_desc_t *pthread, FThreadProc fProc, void *pParam,
                   EThreadSchedPolicy nPolicy, int nPriority)
{
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, nPolicy);
    pthread_attr_getschedparam(&attr, &param);
    param.__sched_priority = nPriority;
    pthread_attr_setschedparam(&attr, &param);

    if (0 == pthread_create((pthread_t*)pthread, NULL, fProc, pParam))
    {
        return TRUE;
    }
    return FALSE;
}

BOOL thread_join(thread_desc_t *pthread, void **ppResult)
{
    return (0 == pthread_join(*pthread, ppResult));
}

thread_desc_t thread_id()
{
    return pthread_self();
}


/*------------------------------------------------------------------------------------*/
void thread_lock_init(thread_lock_t *thread_lock, BOOL bRecursive)
{
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    if (bRecursive)
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    else
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_TIMED_NP);

    pthread_mutex_init(thread_lock, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);
}

BOOL thread_lock_lock(thread_lock_t *thread_lock)
{
    return (0 == pthread_mutex_lock(thread_lock));
}

BOOL thread_lock_try_lock(thread_lock_t *thread_lock)
{
    return (0 == pthread_mutex_trylock(thread_lock));
}

BOOL thread_lock_unlock(thread_lock_t *thread_lock)
{
    return (0 == pthread_mutex_unlock(thread_lock));
}

void thread_lock_destory(thread_lock_t *thread_lock)
{
    pthread_mutex_destroy(thread_lock);
}

/*------------------------------------------------------------------------------------*/
BOOL thread_rwlock_init(thread_rwlock_t *thread_rwlock, U32 type)
{
    int ret;
    pthread_rwlockattr_t rwlock_attr;
    pthread_rwlockattr_init(&rwlock_attr);

    //
    pthread_rwlockattr_setkind_np(&rwlock_attr, type);
    
    ret = pthread_rwlock_init(thread_rwlock, &rwlock_attr);
    pthread_rwlockattr_destroy(&rwlock_attr);
    return (0 == ret);
}

BOOL thread_rwlock_rdlock(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_rdlock(thread_rwlock));
}

BOOL thread_rwlock_try_rdlock(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_tryrdlock(thread_rwlock));
}

BOOL thread_rwlock_wrlock(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_wrlock(thread_rwlock));
}

BOOL thread_rwlock_try_wrlock(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_trywrlock(thread_rwlock));;
}

BOOL thread_rwlock_unlock(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_unlock(thread_rwlock));
}

BOOL thread_rwlock_destory(thread_rwlock_t *thread_rwlock)
{
    return (0 == pthread_rwlock_destroy(thread_rwlock));
}




