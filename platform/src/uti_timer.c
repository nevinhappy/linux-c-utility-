/******************************************************************************

  Copyright (C), 2010-2012, 

 ******************************************************************************
  File Name     : uti_timer.c
  Version       : Initial Draft
  Author        : xiongjunyong
  Created       : 2012/6/8
  Last Modified :
  Description   : timer implementation
  Function List :
              callback_timer_init
              callback_timer_destory
              timer_mgr_add_timer
              timer_mgr_check_timer
              timer_mgr_del_timer
              timer_mgr_init
              timer_mgr_destory
              timer_mgr_size
  History       :
  1.Date        : 2012/6/8
    Author      : xiongjunyong
    Modification: Created file

******************************************************************************/
    
#include "uti_time.h"
#include "uti_timer.h"
#include "uti_logging.h"
#include "uti_mem_mgr.h"

/*****************************************************************************
 Prototype    : uti_timer_cmp
 Description  : timer compare function, support timer end time the same
 Input        : callback_timer_t *ptimer1  
                callback_timer_t *ptimer2  
 Output       : None
 Return Value : ptimer1<ptimer2 return -1, ptimer1=ptimer2 return 0 , 
                ptimer1>ptimer2 return 1
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/7/26
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
static inline int uti_timer_cmp(callback_timer_t *ptimer1, callback_timer_t *ptimer2)
{
    if(ptimer1->timeExpire > ptimer2->timeExpire) {
        return 1;
    } else if(ptimer1->timeExpire < ptimer2->timeExpire) {
        return -1;
    } else {
        /* the one which is added first will great then the later one */
        if(ptimer1->timeBegin < ptimer2->timeBegin) {
            return 1;
        } else if(ptimer1->timeBegin > ptimer2->timeBegin) {
            return -1;
        } else {
            return 0;
        }
    }
}

/*****************************************************************************
 Prototype    : uti_timer_rbtree_search
 Description  : find the timers with timeExpire, if found , will the first one
                timer, if no one exist, return NULL
 Input        : struct rb_root *root  
                I64 timeExpire        
 Output       : None
 Return Value : inline
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/7/26
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
inline callback_timer_t *uti_timer_rbtree_search(struct uti_rb_root *root, I64 timeExpire)
{
  	struct uti_rb_node *pnode = root->rb_node;
    
  	while (pnode) {
  		callback_timer_t *pmynode = container_of(pnode, callback_timer_t, anchor);
        if(timeExpire < pmynode->timeExpire) {
            pnode = pnode->rb_left;
        } else if(timeExpire > pmynode->timeExpire) {
            pnode = pnode->rb_right;
        } else {
  			return pmynode;
        }
	}
	return NULL;
}

inline BOOL uti_timer_rbtree_insert(struct uti_rb_root *root, callback_timer_t *ptimer)
{
    struct uti_rb_node **node_iterator = &(root->rb_node), *parent = NULL;
    
  	while (*node_iterator) {
        parent = *node_iterator;
  		callback_timer_t *cur_node = container_of(*node_iterator, callback_timer_t, anchor);
        if(uti_timer_cmp(ptimer, cur_node) < 0) {
            node_iterator = &((*node_iterator)->rb_left);
        } else if(uti_timer_cmp(ptimer, cur_node) > 0) {
            node_iterator = &((*node_iterator)->rb_right);
        } else {
            // the same <timeBegin, timeExpire> is not allowed to insert twice.
  			return FALSE;
        }
	}
    uti_rb_link_node(&ptimer->anchor, parent, node_iterator);
    uti_rb_insert_color(&ptimer->anchor, root);
	return TRUE;
}

/*****************************************************************************
 Prototype    : raw_timer_init
 Description  : set timer 
 Input        : callback_timer_t* timer    
                U32 ms_expire  : millisecond    
                timer_func timer_fun  
                void* arg
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/6
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/

inline BOOL callback_timer_init(callback_timer_t* timer, 
                       U32 ms_elapse, 
                       timer_func timer_fun,
                       void* arg)
{
    memset(timer, 0x00, sizeof(callback_timer_t));
    timer->msElapse   = ms_elapse;
    timer->onTimer    = timer_fun;
    timer->arg        = arg;
    return TRUE;
}

inline BOOL callback_timer_reset(callback_timer_t* timer)
{
    memset(timer, 0x00, sizeof(callback_timer_t));
    
    return TRUE;
}

inline BOOL timer_mgr_init(timer_manager_t *timer_mgr, size_t wm)
{
    timer_mgr->timer_map = UTI_RB_ROOT;
    timer_mgr->size = 0;
    timer_mgr->capacity = wm;
    return TRUE;
}

/* default timer destory function */
static inline void callback_timer_destory(callback_timer_t* timer)
{
    //LOGGING_INFO("%s", "callback_timer_destory");
    PMEM_FREE(timer);
}

/*****************************************************************************
 Prototype    : timer_mgr_clear
 Description  : Clear all the timers existing in timer manager
 Input        : timer_manager_t *timer_mgr  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2012/7/27
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
BOOL timer_mgr_clear(timer_manager_t *timer_mgr, fn_timer_destory fun_destory)
{
    struct uti_rb_node *node_it;
    callback_timer_t* ptimer;
    //LOGGING_INFO("%s", "timer_mgr_clear");
    //
    for(node_it = uti_rb_first(&timer_mgr->timer_map); node_it&&!UTI_RB_EMPTY_ROOT(&timer_mgr->timer_map);) {
        ptimer = uti_rb_entry(node_it, callback_timer_t, anchor);
        // move to previous node
        node_it = uti_rb_prev(node_it);
        // remove node from rb tree
        uti_rb_erase(&ptimer->anchor, &timer_mgr->timer_map);
        
        /* free timer , if provide @fun_destory, call it, otherwise, use default way */
        if(fun_destory) {
            fun_destory(ptimer);
        } else {
            callback_timer_destory(ptimer);
        }
        
        // check the current node.
        if (NULL == node_it) {      /* if current node is the first one */
            node_it = uti_rb_first(&timer_mgr->timer_map);
            continue;
        } 
        /* normally move to next node */
        node_it = uti_rb_next(node_it);
    }

    //reset size
    timer_mgr->size = 0;
    
    return TRUE;
}

BOOL timer_mgr_destory(timer_manager_t *timer_mgr, fn_timer_destory fun_destory)
{
    //LOGGING_INFO("%s", "timer_mgr_destory");
    //clear all timers 
    timer_mgr_clear(timer_mgr, fun_destory);
    
    return TRUE;
}

BOOL timer_mgr_add_timer(timer_manager_t* timer_mgr, callback_timer_t* timer)
{
    /* check water mark */
    if((timer_mgr->size + 1)>timer_mgr->capacity) {
        return FALSE;
    }

    /* set timer value */
    timer->timeBegin = uti_get_time_s();
    timer->timeExpire = timer->timeBegin + ((I64)timer->msElapse)*1000; /*convert millsecond to microsecond*/
    
    //add timer 
    if(FALSE == uti_timer_rbtree_insert(&timer_mgr->timer_map, timer)) {
        return FALSE;
    }
   
    timer_mgr->size++;
    
    return TRUE;
}

/*
 * NOTE : you should assure , @timer is managed by timer_mgr
 * @fun_destory provid way to free timer
 */
BOOL timer_mgr_del_timer(timer_manager_t* timer_mgr, 
                              callback_timer_t* timer,
                              fn_timer_destory fun_destory)
{
    uti_rb_erase(&timer->anchor, &timer_mgr->timer_map);
    timer_mgr->size--;

    /* free timer */
    if(fun_destory) {
        fun_destory(timer);
    } 
    
    return TRUE;
}

/*****************************************************************************
 Prototype    : timer_mgr_check_timer
 Description  : check all the timer in manager
                NOTE: this function is not thread-safe
 Input        : timer_manager_t* timer_mgr  
 Output       : the timer internal of next time-out 
 Return Value : U32
 Calls        : 
 Called By    : 
 
 History         :
  1.Date         : 2012/6/11
    Author       : xiongjunyong
    Modification : Created function

*****************************************************************************/
U32 timer_mgr_check_timer(timer_manager_t* timer_mgr, fn_timer_destory fun_destory)
{
    //
    struct uti_rb_node *node_it;
    callback_timer_t* ptimer;

    //current time
    I64 cur_time;
    
    //iterator map
    for(node_it = uti_rb_first(&timer_mgr->timer_map); node_it&&!UTI_RB_EMPTY_ROOT(&timer_mgr->timer_map);) {
        ptimer = uti_rb_entry(node_it, callback_timer_t, anchor);
        cur_time = uti_get_time_s();
        //if it's expired
        //LOGGING_INFO("current time :%lld, timer key :%lld ", cur_time, key_temp);
        if(cur_time >= ptimer->timeExpire) {            
            // move to previous node
            node_it = uti_rb_prev(node_it);
            // remove node from rb tree
            uti_rb_erase(&ptimer->anchor, &timer_mgr->timer_map);
            timer_mgr->size--;
            //call fun
            if(ptimer->onTimer) {
                //callback function
                ptimer->onTimer(ptimer->arg, ptimer);
            } 
            if(fun_destory) {
                fun_destory(ptimer);
            }
            // check the current node.
            if (NULL == node_it) {               /* if current node is the first one */
                node_it = uti_rb_first(&timer_mgr->timer_map);
                continue;
            }
            /* normally move to next node */
            node_it = uti_rb_next(node_it);
        }
        /* timer is not time-out , return the time interval */
        return (ptimer->timeExpire - cur_time);
    }
    return TRUE;
}

inline BOOL  timer_mgr_is_empty(timer_manager_t* timer_mgr)
{
    return UTI_RB_EMPTY_ROOT(&timer_mgr->timer_map);
}

inline size_t timer_mgr_size(timer_manager_t* timer_mgr)
{
    return timer_mgr->size;
}

//file end

