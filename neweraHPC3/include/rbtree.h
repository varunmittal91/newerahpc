/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _RBTREE_NEW_H_
#define _RBTREE_NEW_H_

#include "constants.h"

namespace neweraHPC
{
   struct rb_node
   {
      rb_node        *rb_parent;
      short int       rb_color;
#define RB_RED               0
#define RB_BLACK             1
      struct rb_node *rb_right;
      struct rb_node *rb_left;
      
      void *data;
      
      union key_t
      {
	 int	     num;
	 const char *str;	 
      };
      key_t key;
   };
   
#define rb_color(r)         ((r)->rb_color)
#define rb_is_red(r)        (!rb_color(r))
#define rb_is_black(r)      rb_color(r)
#define rb_set_red(r)       ((r)->rb_color = RB_RED)
#define rb_set_black(r)     ((r)->rb_color = RB_BLACK)
#define rb_parent(r)        ((r)->rb_parent)
#define rb_set_parent(r, p) ((r)->rb_parent = p)    
  
   enum RBTREE_MODES
   {
      RBTREE_NUM,
      RBTREE_STR,
      RBTREE_NUM_MANAGED,
      RBTREE_NUM_HASH
   };

   class rbtree
   {
   private:
      rb_node *root;
      
      int count;
      int last_assigned_key;
      int operation_mode;
      
      static inline void rb_link_node(rb_node *new_node, rb_node *parent, rb_node **rb_link);
      void rb_erase(rb_node *node);
      void rb_insert_color(rb_node *new_node);
      void rb_erase_color(rb_node *node, rb_node *parent);

      void __rb_rotate_left(rb_node  *node);
      void __rb_rotate_right(rb_node *node);
      void __rb_print(rb_node *node);      
      
      nhpc_status_t insert_node(rb_node *node);
      rb_node *search_node(int *key_num, const char **key_str);
      rb_node *search_node(int pos);
      void erase_node(rb_node *node);

      rb_node *rb_first();
      rb_node *rb_last();
      rb_node *rb_next(const rb_node *node);
   public:
       rbtree(int _operation_mode = RBTREE_NUM);
      ~rbtree();
      
      nhpc_status_t insert(void *data);
      nhpc_status_t insert(void *data, int key);
      nhpc_status_t insert(void *data, const char *key);
      
      void *search(int key);
      void *search(const char *key);
      void *search_inorder(int pos, const char **key);
      void *search_inorder(int pos, int *key);
      void *operator[](int pos);

      int length();
      
      nhpc_status_t erase(int key);
      nhpc_status_t erase(const char *key);
      nhpc_status_t erase_inorder(int pos);

      void print_tree();
   };
};

#endif