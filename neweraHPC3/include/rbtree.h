/*
 *  Original Author
 *	(C) 1999  Andrea Arcangeli <andrea@suse.de>
 *
 *	Modified by
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
 *
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v2
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 2 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef	_RBTREE_H_
#define	_RBTREE_H_

#include <stddef.h>
#include <stdio.h>
#include <cstddef>
#include <pthread.h>

#include "strings.h"
#include "constants.h"

#define container_of(ptr, type, member) ({            \
const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
(type *)( (char *)__mptr - offsetof(type,member) );})

struct rb_node
{
   unsigned long  rb_parent_color;
#define	RB_RED		0
#define	RB_BLACK	1
   struct rb_node *rb_right;
   struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root
{
   struct rb_node *rb_node;
};


#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)   ((r)->rb_parent_color & 1)
#define rb_is_red(r)   (!rb_color(r))
#define rb_is_black(r) rb_color(r)
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
   rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static inline void rb_set_color(struct rb_node *rb, int color)
{
   rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

typedef void (*rb_augment_f)(struct rb_node *node, void *data);

extern void rb_augment_insert(struct rb_node *node,
			      rb_augment_f func, void *data);
extern struct rb_node *rb_augment_erase_begin(struct rb_node *node);
extern void rb_augment_erase_end(struct rb_node *node,
				 rb_augment_f func, void *data);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new_node, struct rb_root *root);

#ifndef NULL
#define NULL 0
#endif

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,	struct rb_node ** rb_link)
{
   node->rb_parent_color = (unsigned long )parent;
   node->rb_left = node->rb_right = NULL;
   
   *rb_link = node;
}

#define NHPC_RBTREE_NUM 0
#define NHPC_RBTREE_STR 1

namespace neweraHPC
{
   struct key_pair_t
   {
      char *key;
      void *data;
   };
   
   class rbtree_t
   {
   private:
      rb_root *root;
      struct node
      {
	 struct rb_node node_next;
	 void *node_data;
	 int  node_key;
	 char *node_key_str;
      };
      int last_assigned_key;
      node *search_node(int node_key);
      node *search_node(const char *node_key_str);
      int count;
      bool num_mode;
      
   public:
      rbtree_t();
      rbtree_t(int mode);
      ~rbtree_t();
      void *search(int key);
      void *search(const char *key_str);
      key_pair_t *search_str(int key);
      int insert(void *);
      int insert(void *, int key);
      int insert(void *, const char *key_str);
      int erase(int key);
      int erase(const char *key_str);
      int update(int, void *);
      int ret_count();
   };
};

#endif
