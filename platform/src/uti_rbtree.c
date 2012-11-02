/**
 * @file
 * @brief reb-black tree c implementation
 *
 * @note borrow from Linux lib/rbtree.c
 */
#include "platform.h"

#include "uti_rbtree.h"

static void __rb_rotate_left(struct uti_rb_node *node, struct uti_rb_root *root)
{
    struct uti_rb_node *right = node->rb_right;
    struct uti_rb_node *parent = uti_rb_parent(node);

    if ((node->rb_right = right->rb_left)) uti_rb_set_parent(right->rb_left, node);
    right->rb_left = node;

    uti_rb_set_parent(right, parent);

    if (parent) {
        if (node == parent->rb_left)
            parent->rb_left = right;
        else
            parent->rb_right = right;
    } else {
        root->rb_node = right;
    }
    uti_rb_set_parent(node, right);
}

static void __rb_rotate_right(struct uti_rb_node *node, struct uti_rb_root *root)
{
    struct uti_rb_node *left = node->rb_left;
    struct uti_rb_node *parent = uti_rb_parent(node);

    if ((node->rb_left = left->rb_right)) uti_rb_set_parent(left->rb_right, node);
    left->rb_right = node;

    uti_rb_set_parent(left, parent);

    if (parent) {
        if (node == parent->rb_right)
            parent->rb_right = left;
        else
            parent->rb_left = left;
    } else {
        root->rb_node = left;
    }
    uti_rb_set_parent(node, left);
}

void uti_rb_insert_color(struct uti_rb_node *node, struct uti_rb_root *root)
{
    struct uti_rb_node *parent, *gparent;

    while ((parent = uti_rb_parent(node)) && uti_rb_is_red(parent)) {
        gparent = uti_rb_parent(parent);

        if (parent == gparent->rb_left) {
            {
                register struct uti_rb_node *uncle = gparent->rb_right;
                if (uncle && uti_rb_is_red(uncle)) {
                    uti_rb_set_black(uncle);
                    uti_rb_set_black(parent);
                    uti_rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_right == node) {
                register struct uti_rb_node *tmp;
                __rb_rotate_left(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            uti_rb_set_black(parent);
            uti_rb_set_red(gparent);
            __rb_rotate_right(gparent, root);
        } else {
            {
                register struct uti_rb_node *uncle = gparent->rb_left;
                if (uncle && uti_rb_is_red(uncle)) {
                    uti_rb_set_black(uncle);
                    uti_rb_set_black(parent);
                    uti_rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            if (parent->rb_left == node) {
                register struct uti_rb_node *tmp;
                __rb_rotate_right(parent, root);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            uti_rb_set_black(parent);
            uti_rb_set_red(gparent);
            __rb_rotate_left(gparent, root);
        }
    }

    uti_rb_set_black(root->rb_node);
}

static void __rb_erase_color(struct uti_rb_node *node, struct uti_rb_node *parent, struct uti_rb_root *root)
{
    struct uti_rb_node *other;

    while ((!node || uti_rb_is_black(node)) && node != root->rb_node) {
        if (parent->rb_left == node) {
            other = parent->rb_right;
            if (uti_rb_is_red(other)) {
                uti_rb_set_black(other);
                uti_rb_set_red(parent);
                __rb_rotate_left(parent, root);
                other = parent->rb_right;
            }
            if ((!other->rb_left || uti_rb_is_black(other->rb_left)) &&
                (!other->rb_right || uti_rb_is_black(other->rb_right))) {
                uti_rb_set_red(other);
                node = parent;
                parent = uti_rb_parent(node);
            } else {
                if (!other->rb_right || uti_rb_is_black(other->rb_right)) {
                    struct uti_rb_node *o_left;
                    if ((o_left = other->rb_left)) uti_rb_set_black(o_left);
                    uti_rb_set_red(other);
                    __rb_rotate_right(other, root);
                    other = parent->rb_right;
                }
                uti_rb_set_color(other, uti_rb_color(parent));
                uti_rb_set_black(parent);
                if (other->rb_right) uti_rb_set_black(other->rb_right);
                __rb_rotate_left(parent, root);
                node = root->rb_node;
                break;
            }
        } else {
            other = parent->rb_left;
            if (uti_rb_is_red(other)) {
                uti_rb_set_black(other);
                uti_rb_set_red(parent);
                __rb_rotate_right(parent, root);
                other = parent->rb_left;
            }
            if ((!other->rb_left || uti_rb_is_black(other->rb_left)) &&
                (!other->rb_right || uti_rb_is_black(other->rb_right))) {
                uti_rb_set_red(other);
                node = parent;
                parent = uti_rb_parent(node);
            } else {
                if (!other->rb_left || uti_rb_is_black(other->rb_left)) {
                    register struct uti_rb_node *o_right;
                    if ((o_right = other->rb_right)) uti_rb_set_black(o_right);
                    uti_rb_set_red(other);
                    __rb_rotate_left(other, root);
                    other = parent->rb_left;
                }
                uti_rb_set_color(other, uti_rb_color(parent));
                uti_rb_set_black(parent);
                if (other->rb_left) uti_rb_set_black(other->rb_left);
                __rb_rotate_right(parent, root);
                node = root->rb_node;
                break;
            }
        }
    }
    if (node) uti_rb_set_black(node);
}

void uti_rb_erase(struct uti_rb_node *node, struct uti_rb_root *root)
{
    struct uti_rb_node *child, *parent;
    int color;

    if (!node->rb_left) child = node->rb_right;
    else if (!node->rb_right) child = node->rb_left;
    else {
        struct uti_rb_node *old = node, *left;

        node = node->rb_right;
        while ((left = node->rb_left) != NULL) node = left;
        child = node->rb_right;
        parent = uti_rb_parent(node);
        color = uti_rb_color(node);

        if (child) uti_rb_set_parent(child, parent);
        if (parent == old) {
            parent->rb_right = child;
            parent = node;
        } else {
            parent->rb_left = child;
        }

        node->rb_parent_color = old->rb_parent_color;
        node->rb_right = old->rb_right;
        node->rb_left = old->rb_left;

        if (uti_rb_parent(old)) {
            if (uti_rb_parent(old)->rb_left == old)
                uti_rb_parent(old)->rb_left = node;
            else
                uti_rb_parent(old)->rb_right = node;
        } else {
            root->rb_node = node;
        }

        uti_rb_set_parent(old->rb_left, node);
        if (old->rb_right) uti_rb_set_parent(old->rb_right, node);
        goto color;
    }

    parent = uti_rb_parent(node);
    color = uti_rb_color(node);

    if (child) uti_rb_set_parent(child, parent);
    if (parent) {
        if (parent->rb_left == node)
            parent->rb_left = child;
        else
            parent->rb_right = child;
    } else {
        root->rb_node = child;
    }

 color:
    if (color == RB_BLACK) __rb_erase_color(child, parent, root);
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
inline struct uti_rb_node *uti_rb_first(struct uti_rb_root *root)
{
    struct uti_rb_node    *n;

    n = root->rb_node;
    if (!n) return NULL;
    while (n->rb_left) n = n->rb_left;
    return n;
}

inline struct uti_rb_node *uti_rb_last(struct uti_rb_root *root)
{
    struct uti_rb_node    *n;

    n = root->rb_node;
    if (!n) return NULL;
    while (n->rb_right) n = n->rb_right;
    return n;
}

inline struct uti_rb_node *uti_rb_next(struct uti_rb_node *node)
{
    struct uti_rb_node *parent;

    if (uti_rb_parent(node) == node) return NULL;

    /* If we have a right-hand child, go down and then left as far
       as we can. */
    if (node->rb_right) {
        node = node->rb_right;
        while (node->rb_left) node=node->rb_left;
        return node;
    }

    /* No right-hand children.  Everything down and left is
       smaller than us, so any 'next' node must be in the general
       direction of our parent. Go up the tree; any time the
       ancestor is a right-hand child of its parent, keep going
       up. First time it's a left-hand child of its parent, said
       parent is our 'next' node. */
    while ((parent = uti_rb_parent(node)) && node == parent->rb_right) node = parent;

    return parent;
}

inline struct uti_rb_node *uti_rb_prev(struct uti_rb_node *node)
{
    struct uti_rb_node *parent;

    if (uti_rb_parent(node) == node) return NULL;

    /* If we have a left-hand child, go down and then right as far
       as we can. */
    if (node->rb_left) {
        node = node->rb_left;
        while (node->rb_right) node=node->rb_right;
        return node;
    }

    /* No left-hand children. Go up till we find an ancestor which
       is a right-hand child of its parent */
    while ((parent = uti_rb_parent(node)) && node == parent->rb_left) node = parent;

    return parent;
}

void uti_rb_replace_node(struct uti_rb_node *victim, struct uti_rb_node *new, struct uti_rb_root *root)
{
    struct uti_rb_node *parent = uti_rb_parent(victim);

    /* Set the surrounding nodes to point to the replacement */
    if (parent) {
        if (victim == parent->rb_left)
            parent->rb_left = new;
        else
            parent->rb_right = new;
    } else {
        root->rb_node = new;
    }
    if (victim->rb_left) uti_rb_set_parent(victim->rb_left, new);
    if (victim->rb_right) uti_rb_set_parent(victim->rb_right, new);

    /* Copy the pointers/colour from the victim to the replacement */
    *new = *victim;
}
