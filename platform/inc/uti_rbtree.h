/**
 * @file
 * @brief 
 *
 * @note borrow from Linux include/linux/rbtree.h
 */
#ifndef __UTI_RBTREE_H__
#define __UTI_RBTREE_H__

#undef offsetof
#ifdef __compiler_offsetof
# define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)     //!<
#else
# define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)    //!< 
#endif

#define container_of(ptr, type, member) ({          \
    typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})


struct uti_rb_node {
    unsigned long  rb_parent_color;
#define RB_RED      0
#define RB_BLACK    1
    struct uti_rb_node *rb_right;
    struct uti_rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

struct uti_rb_root {
    struct uti_rb_node *rb_node;
};

#define uti_rb_parent(r)    ((struct uti_rb_node *)((r)->rb_parent_color & ~3))
#define uti_rb_color(r)     ((r)->rb_parent_color & 1)
#define uti_rb_is_red(r)    (!uti_rb_color(r))
#define uti_rb_is_black(r)  uti_rb_color(r)
#define uti_rb_set_red(r)   do { (r)->rb_parent_color &= ~1; } while (0)
#define uti_rb_set_black(r) do { (r)->rb_parent_color |= 1; } while (0)

static inline void uti_rb_set_parent(struct uti_rb_node *rb, struct uti_rb_node *p)
{
    rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}

static inline void uti_rb_set_color(struct uti_rb_node *rb, int color)
{
    rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

static inline void uti_rb_link_node(struct uti_rb_node * node, struct uti_rb_node * parent, struct uti_rb_node ** rb_link)
{
    node->rb_parent_color = (unsigned long )parent;
    node->rb_left = node->rb_right = NULL;

    *rb_link = node;
}

#define UTI_RB_ROOT                     (struct uti_rb_root) { NULL, }
#define uti_rb_entry(ptr, type, member) container_of(ptr, type, member)

#define UTI_RB_EMPTY_ROOT(root)    ((root)->rb_node == NULL)
#define UTI_RB_EMPTY_NODE(node)    (uti_rb_parent(node) == node)
#define UTI_RB_CLEAR_NODE(node)    (uti_rb_set_parent(node, node))

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void uti_rb_insert_color(struct uti_rb_node *, struct uti_rb_root *);
void uti_rb_erase(struct uti_rb_node *, struct uti_rb_root *);

/* Find logical next and previous nodes in a tree */
inline struct uti_rb_node *uti_rb_next(struct uti_rb_node *);
inline struct uti_rb_node *uti_rb_prev(struct uti_rb_node *);
inline struct uti_rb_node *uti_rb_first(struct uti_rb_root *);
inline struct uti_rb_node *uti_rb_last(struct uti_rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void uti_rb_replace_node(struct uti_rb_node *victim, struct uti_rb_node *new, struct uti_rb_root *root);

#define UTI_RB_INSERT_FUNC(type, cmp )   \
inline BOOL uti_rbtree_insert(struct uti_rb_root *root, type *ptimer) {     \
    struct uti_rb_node **node_iterator = &(root->rb_node), *parent = NULL;  \
  	while (*node_iterator) {                                            \
        parent = *node_iterator;                                        \
  		type *cur_node = container_of(*node_iterator, type, anchor);    \
        if(cmp(ptimer, cur_node) < 0) {                                 \
            node_iterator = &((*node_iterator)->rb_left);               \
        } else if(uti_timer_cmp(ptimer, cur_node) > 0) {                \
            node_iterator = &((*node_iterator)->rb_right);              \
        } else {                                                        \
  			return FALSE; }}                                            \
    rb_link_node(&ptimer->anchor, parent, node_iterator);               \
    uti_rb_insert_color(&ptimer->anchor, root);                             \
	return TRUE; }


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__UTI_RBTREE_H__*/

