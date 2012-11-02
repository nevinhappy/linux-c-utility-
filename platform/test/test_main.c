// test_main.c : test platform functions and utilities.

#include "platform.h"
#include "uti_time.h"
#include "uti_logging.h"
#include "uti_thread.h"
#include "uti_rbtree.h"
#include "uti_common.h"
#include "uti_timer.h"
#include "uti_mem_mgr.h"

int g_test_data = 0;
thread_lock_t g_lock;

void callback_timer_destory(callback_timer_t *timer){
    PMEM_FREE(timer);
}

void* timer_callback_fun(void* args, callback_timer_t* timer)
{
    int* temp = (int*)args;
    if(NULL == args)
    {
        return NULL;
    }
    LOGGING_INFO(" timer_callback_fun timeout, ID : %d ", *temp);
    callback_timer_destory(timer);
    return NULL;
}

#define TEST_NUM    (20)
void test_timer()
{
    int index;
    int array[TEST_NUM];
    callback_timer_t* ptimer;
    timer_manager_t timer_mgt;
    int time_wait;
    
    if(!timer_mgr_init(&timer_mgt, 10))
    {
        LOGGING_ERROR("timer manager init failed .");
        return ;
    }
    LOGGING_INFO("timer manager inited");

    for(index = 0; index<TEST_NUM; index++)
    {
        array[index] = index;
        ptimer = (callback_timer_t *)PMEM_ALLOC(sizeof(callback_timer_t));
        if(NULL == ptimer)
        {
            LOGGING_ERROR("malloc timer failed .");
            continue;
        }
        //
        time_wait = os_random(10000);
        if(time_wait<1000) time_wait = 3000;
        LOGGING_INFO("Timer index : %d is created, ID:%d, time-out : %d.", index, array[index], time_wait);
        if(callback_timer_init(ptimer, time_wait, timer_callback_fun, &(array[index])))
        {
            //LOGGING_INFO("callback_timer inited");
            if(!timer_mgr_add_timer(&timer_mgt, ptimer))
            {
                PMEM_FREE(ptimer);
                LOGGING_ERROR("%s ", "add timer failed .");
                continue;
            }
            LOGGING_INFO("callback_timer added , size : %zu", timer_mgt.size);
        }
        //os_sleep(100);
    }
    
    while(!timer_mgr_is_empty(&timer_mgt))
    {
        //check timers
        time_wait = timer_mgr_check_timer(&timer_mgt, NULL)/1000 - 4;
        if(time_wait > 0) {
            LOGGING_INFO("time_wait : %d, size : %zu", time_wait, timer_mgt.size);
            os_sleep(time_wait);
        } 
    }
    timer_mgr_destory(&timer_mgt, callback_timer_destory);
    LOGGING_ERROR("current time : %lld", uti_get_time_s());
    LOGGING_ERROR("current time : %lld", uti_get_time_s());
    //
}


typedef struct TreeNode_s {
    struct uti_rb_node node;
    int data;
}TreeNode_t;

TreeNode_t *MY_RB_search(struct uti_rb_root *root, int data)
{
  	struct uti_rb_node *pnode = root->rb_node;
    LOGGING_INFO("MY_RB_search entry :");
    
  	while (pnode) {
  		TreeNode_t *pmynode = container_of(pnode, TreeNode_t, node);
        if(data < pmynode->data) {
            pnode = pnode->rb_left;
        } else if(data > pmynode->data) {
            pnode = pnode->rb_right;
        } else {
            LOGGING_INFO("MY_RB_search find : %p", pmynode);
  			return pmynode;
        }
	}
    LOGGING_WARN("MY_RB_search not find!");
	return NULL;
}

int MY_RB_insert(struct uti_rb_root *root, TreeNode_t *pdata)
{
    struct uti_rb_node **temp_node = &(root->rb_node), *parent = NULL;
    
  	while (*temp_node) {
        parent = *temp_node;
  		TreeNode_t *this = container_of(*temp_node, TreeNode_t, node);
        if(pdata->data < this->data) {
            temp_node = &((*temp_node)->rb_left);
        } else if(pdata->data > this->data) {
            temp_node = &((*temp_node)->rb_right);
        } else {
  			return -1;
        }
	}
    uti_rb_link_node(&pdata->node, parent, temp_node);
    uti_rb_insert_color(&pdata->node, root);
	return 1;
}
#define TEST_RB_NODE_NUM     10
int test_rbtree() 
{
    int index;
    TreeNode_t *pnode;

    LOGGING_INFO("test_rbtree entry :");
    struct uti_rb_node *node_it;
    //struct rb_node *node_it_temp;
    //
    struct uti_rb_root root = UTI_RB_ROOT;
    LOGGING_INFO("uti_rb_first : %p", uti_rb_first(&root));
    LOGGING_INFO("uti_rb_last : %p", uti_rb_last(&root));

    //add nodes
    LOGGING_INFO("rbtree add nodes.");
    for(index=0; index<TEST_RB_NODE_NUM; index++ ) {
        pnode = (TreeNode_t*)PMEM_ALLOC(sizeof(TreeNode_t));
        pnode->data = os_random(1000);
        //insert node to tree
        MY_RB_insert(&root, pnode);
        LOGGING_INFO("rbtree add node: %p", pnode);
    }

    //access nodes
    for (node_it = uti_rb_first(&root); node_it; node_it = uti_rb_next(node_it)) {
        LOGGING_INFO("key=%d\n", uti_rb_entry(node_it, TreeNode_t, node)->data);
    }

    //del nodes
    /*for(index=0; index<TEST_RB_NODE_NUM; index++ ) { 
        pnode = MY_RB_search(&root, index);
        if(pnode) {
            LOGGING_INFO("Find node , data : %d, delete from tree.", pnode->data);
            rb_erase(&pnode->node, &root);
            PMEM_FREE(pnode);            
        }
    }*/
    LOGGING_INFO("uti_rb_first : %p", uti_rb_first(&root));
    LOGGING_INFO("uti_rb_last : %p", uti_rb_last(&root)); 
    
    for (node_it = uti_rb_first(&root); node_it&&!UTI_RB_EMPTY_ROOT(&root); ) {
        pnode = uti_rb_entry(node_it, TreeNode_t, node);
        node_it = uti_rb_prev(node_it);
        uti_rb_erase(&pnode->node, &root);
        PMEM_FREE(pnode); 
        LOGGING_INFO("Find node , data : %d, delete from tree.", pnode->data);
        if(NULL == node_it) {
            node_it = uti_rb_first(&root);
            continue;
        }
        node_it = uti_rb_next(node_it);
    }

    return 0;
}

int test_ip_convert()
{
    ipv4_addr_t ip_addr;
    ip_addr.addr1 = 10;
    ip_addr.addr2 = 67;
    ip_addr.addr3 = 1;
    ip_addr.addr4 = 41;
    LOGGING_INFO("IP addr : %d", ip_addr.value);
    return 0;
}
int main(int argc, char *argv[])
{
    //LOGGING_BASIC_INIT(logInfo, default_log_agent);    

    //call test cases
    //thread_lock_init(&g_lock, FALSE);
    //
    //test_uti_logging();
    //test_thread_fun();
    //test_lists();
    //test_mem_alloc();
    //
    //test_rbtree();
    //
    test_timer();
    //
    //LOGGING_CRITICAL("LOGGING_CRITICAL");
    //LOGGING_ERROR("LOGGING_ERROR");
    //LOGGING_WARN("LOGGING_WARN");
    //LOGGING_INFO("LOGGING_WARN");
    //test_ip_convert();
    //
    //
    //thread_lock_destory(&g_lock);
    //end process    
    return 0;
}



