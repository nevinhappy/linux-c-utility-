#ifndef __UTI_TIMER_H__
#define __UTI_TIMER_H__

#include "platform.h"

#include "uti_rbtree.h"
//
#ifdef __cplusplus
extern "C" {
#endif //:__cplusplus

struct callback_timer_s;
/*
 * call back function pointer define 
 */
typedef void*   (*timer_func)(void *, struct callback_timer_s*);

/*
 * timer struct define 
 */
typedef struct callback_timer_s {
    struct uti_rb_node  anchor;         //<! rb tree node anchor
    I64             timeBegin;      //<! the time when setting this timer added to mgr
    I64             timeExpire;     //<! the time when setting this timer time-out
    U32             msElapse;       //<! micro seconds will elapse before time-out
    timer_func      onTimer;        //<! when timer expired, this function will be called
    void*           arg;            //<! save the arguments to onTimer function
}callback_timer_t;

inline BOOL callback_timer_init(callback_timer_t* timer, U32 ms_expire, timer_func timer_fun, void* arg);

inline BOOL callback_timer_reset(callback_timer_t* timer);

/**
 * timer free function define 
 * NOTE : callback timer memory management is open.
 **/
typedef void (*fn_timer_destory)(callback_timer_t*);

/*
 * timer manager struct define
 */
#define DEFAULT_TIMER_WATERMARK     (-1)
typedef struct timer_manager_s {
    size_t                  capacity;           //<! record the max size of timers in manager
    size_t                  size;               //<! record current size
    struct uti_rb_root      timer_map;          //<! use rbtree to store timer    
}timer_manager_t;

/**
 * FUNC     :timer_mgr_init
 * PARAM    :@timer_mgr "timer manager which need to be inited "
 *           @wm " setting the watermark of @timer_mgr"
 * DESC     : init @timer_mgr
 **/
inline BOOL timer_mgr_init(timer_manager_t *timer_mgr, size_t wm);

/**
 * FUNC     :timer_mgr_destory
 * PARAM    :@timer_mgr " destory @timer_mgr "
 *           @fun_destory " provide the way of freeing timers , if NULL, will 
                use the default way to free ."
 * DESC     : clear all timers existing in @timer_mgr and destory @timer_mgr
 **/
BOOL timer_mgr_destory(timer_manager_t *timer_mgr, fn_timer_destory fun_destory);

/**
 * FUNC     :timer_mgr_clear
 * PARAM    :@timer_mgr " clear all timers in @timer_mgr "
 *           @fun_destory " provide the way of freeing timers , if NULL, will 
                use the default way to free ."
 * DESC     : clear all timers existing in @timer_mgr
 **/
BOOL timer_mgr_clear(timer_manager_t *timer_mgr, fn_timer_destory fun_destory);

/**
 * FUNC     :timer_mgr_add_timer
 * PARAM    :@timer_mgr " add a @timer to @timer_mgr "
 *           @timer " timer pointer"
 * DESC     : add a @timer to @timer_mgr
 **/
BOOL timer_mgr_add_timer(timer_manager_t* timer_mgr, callback_timer_t* timer);

/**
 * FUNC     :timer_mgr_del_timer
 * PARAM    :@timer_mgr " add a @timer to @timer_mgr "
 *           @timer     " timer pointer"
 *           @fun_destory " the free function for destory @timer"
 * DESC     : delete a @timer from @timer_mgr, if @fun_destory is not NULL, 
 *           this timer will be destory in @fun_destory way, otherwise, 
 *           @timer is not freed.
 **/
BOOL timer_mgr_del_timer(timer_manager_t* timer_mgr, 
                              callback_timer_t* timer, 
                              fn_timer_destory fun_destory);
/**
 * FUNC     :timer_mgr_check_timer
 * PARAM    :@timer_mgr " timer manager which is going to be checked. "
 *           @fun_destory " the free function for destorying a timer"
 * DESC     : the logic of checking whether timers in @timer_mgr is time-out,
 *           if time-out, then call the callback func and remove timer from
 *          @timer_mgr, if fun_destory is not NULL, then timer will be destoryed
 *          if NULL, should conside the freeing of timers.
 **/
U32 timer_mgr_check_timer(timer_manager_t* timer_mgr, fn_timer_destory fun_destory);

/*
 * check whether timer manager is empty, empty :TRUE, else FALSE
 */
inline BOOL  timer_mgr_is_empty(timer_manager_t* timer_mgr);

/*
 * get the current number of timers
 */
size_t timer_mgr_size(timer_manager_t* timer_mgr);


//
#ifdef __cplusplus
}
#endif //:__cplusplus


#endif /* __UTI_TIMER_H__ */

