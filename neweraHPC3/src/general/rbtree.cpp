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

#include <string.h>
#include <iostream>

#include <include/rbtree.h>
#include <include/constants.h>
#include <include/strings.h>
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   rbtree::rbtree(int _operation_mode)
   {
      root		= NULL;      
      count             = 0;
      last_assigned_key = 0;
      operation_mode    = _operation_mode;
   }
   
   rbtree::~rbtree()
   {
      rb_node *node = rb_first(), *tmp = NULL;

      while(node || tmp)
      {
	 if(tmp)
	 {
	    erase_node(tmp);
	 }
	 tmp  = node;
	 if(node)
	    node = rb_next(node);
      }
   }
   
   int rbtree::length()
   {
      return count;
   }
   
   nhpc_status_t rbtree::insert_node(rb_node *new_node)
   {
      rb_node **node = &(root), *parent = NULL;
      int key;
      
      while(*node)
      {
	 parent = *node;
	 
	 if(operation_mode == RBTREE_STR)
	    key = strcmp(new_node->key.str, ((*node)->key.str));
	 else 
	    key = (new_node->key.num) - (*node)->key.num;
	 
	 if(key < 0)
	    node = &((*node)->rb_left);
	 else if(key > 0)
	    node = &((*node)->rb_right);
	 else 
	 {
	    LOG_ERROR("RBTREE Insertion Failed, Key Exists");
	    return NHPC_FAIL;
	 }
      }
      
      rb_link_node(new_node, parent, node);
      rb_insert_color(new_node);
      
      count++;
      
      return NHPC_SUCCESS;
   }
   
   rb_node *rbtree::search_node(int *key_num, const char **key_str)
   {
      rb_node *node = root;
      int key;
      
      while(node)
      {
	 if(key_str)
	    key = strcmp(*key_str, node->key.str);
	 else 
	    key = *key_num - node->key.num;
	 
	 if(key < 0)
	    node = node->rb_left;
	 else if(key > 0)
	    node = node->rb_right;
	 else 
	    break;
      }
      
      if(!node)
      {
	 if(key_str)
	    LOG_DEBUG("RBTREE Search Failed, Key:" << *key_str);
	 else 
	    LOG_DEBUG("RBTREE Search Failed, Key:" << *key_num);	    
      }
      
      return node;
   }
   
   rb_node *rbtree::search_node(int pos)
   {
      rb_node *node = NULL;
      for(int i = 1; i <= pos; i++)
      {
	 if(!node)
	    node = rb_first();
	 else 
	    node = rb_next(node);
      }
      
      if(!node)
	 LOG_DEBUG("RBTREE Inorder Search Failed, Position:" << pos);
      
      return node;
   }
   
   void rbtree::erase_node(rb_node *node)
   {
      rb_erase(node);
      
      if(operation_mode == RBTREE_STR)
	 delete[] node->key.str;
      count--;
      
      delete node;
   }
   
   nhpc_status_t rbtree::insert(void *data)
   {
      if(operation_mode == RBTREE_STR)
	 return NHPC_FAIL;
            
      if(operation_mode == RBTREE_STR)
	 return NHPC_FAIL;
      
      return insert(data, (last_assigned_key + 1));
   }
   
   nhpc_status_t rbtree::insert(void *data, int key)
   {
      if(operation_mode == RBTREE_STR)
	 return NHPC_FAIL;
      
      rb_node *new_node = new rb_node;
      memset(new_node, 0, sizeof(rb_node));
      new_node->data = data;
      new_node->key.num = key;
      
      nhpc_status_t nrv;
      nrv = insert_node(new_node);
      if(key > last_assigned_key)
	 last_assigned_key = key;
      
      if(nrv == NHPC_FAIL)
      {
	 delete new_node;
	 return NHPC_FAIL;
      }
      
      return (new_node->key.num);
   }
   
   nhpc_status_t rbtree::insert(void *data, const char *key)
   {
      if(operation_mode != RBTREE_STR)
	 return NHPC_FAIL;
            
      rb_node *new_node = new rb_node;
      memset(new_node, 0, sizeof(rb_node));
      new_node->data = data;
      nhpc_strcpy((char **)&(new_node->key.str), key);
      
      nhpc_status_t nrv;
      nrv = insert_node(new_node);

      if(nrv == NHPC_FAIL)
      {
	 delete[] new_node->key.str;
	 delete   new_node;
      }
      
      return nrv;
   }
   
   void *rbtree::search(int key)
   {
      if(operation_mode == RBTREE_STR)
	 return NULL;
      
      rb_node *node = search_node(&key, NULL);
      if(node)
	 return node->data;
      else 
	 return NULL;
   }
   
   void *rbtree::search(const char *key)
   {
      if(operation_mode != RBTREE_STR)
	 return NULL;
      
      rb_node *node = search_node(NULL, &key);
      if(node)
	 return node->data;
      else 
	 return NULL;      
   }
   
   void *rbtree::search_inorder_str(int pos, const char **key)
   {
      if(operation_mode != RBTREE_STR)
	 return NULL;
      
      rb_node *node = search_node(pos);
      
      if(node)
      {
	 if(key)
	    *key = (node->key.str);
	 return node->data;
      }
      else 
	 return NULL;
   }
   
   void *rbtree::search_inorder_num(int pos, int *key)
   {
      if(operation_mode == RBTREE_STR)
	 return NULL;
      
      rb_node *node = search_node(pos);
      
      if(node)
      {
	 if(key)
	    *key = node->key.num;
	 return node->data;
      }
      else 
	 return NULL;
   }
   
   void *rbtree::operator[](int pos)
   {
      rb_node *node = search_node(pos);
      if(node)
	 return node->data;
      else 
	 return NULL;
   }
   
   nhpc_status_t rbtree::erase(int key)
   {
      if(operation_mode == RBTREE_STR)
	 return NHPC_FAIL;
      
      rb_node *node = search_node(&key, NULL);
      if(node)
      {
	 erase_node(node);
	 return NHPC_SUCCESS;
      }
      
      return NHPC_FAIL;
   }
   
   nhpc_status_t rbtree::erase(const char *key)
   {
      if(operation_mode != RBTREE_STR)
	 return NHPC_FAIL;
      
      rb_node *node = search_node(NULL, &key);
      if(node)
      {
	 erase_node(node);
	 return NHPC_SUCCESS;
      }
      
      return NHPC_FAIL;
   }
   
   nhpc_status_t rbtree::erase_inorder(int pos)
   {
      rb_node *node = search_node(pos);
      
      if(node)
      {
	 erase_node(node);
	 return NHPC_SUCCESS;
      }
      
      return NHPC_FAIL;
   }
   
   void rbtree::rb_link_node(rb_node *new_node, rb_node *parent, rb_node **rb_link)
   {
      if(parent)
      {
	 new_node->rb_parent = parent;
      }
      
      *rb_link = new_node;
   }
   
   void rbtree::__rb_rotate_left(struct rb_node *node)
   {
      struct rb_node *right = node->rb_right;
      struct rb_node *parent = rb_parent(node);
      
      if ((node->rb_right = right->rb_left))
	 rb_set_parent(right->rb_left, node);
      right->rb_left = node;
      
      rb_set_parent(right, parent);
      
      if (parent)
      {
	 if (node == parent->rb_left)
	    parent->rb_left = right;
	 else
	    parent->rb_right = right;
      }
      else
	 root = right;
      rb_set_parent(node, right);
   }
   
   void rbtree::__rb_rotate_right(struct rb_node *node)
   {
      struct rb_node *left = node->rb_left;
      struct rb_node *parent = rb_parent(node);
      
      if ((node->rb_left = left->rb_right))
	 rb_set_parent(left->rb_right, node);
      left->rb_right = node;
      
      rb_set_parent(left, parent);
      
      if (parent)
      {
	 if (node == parent->rb_right)
	    parent->rb_right = left;
	 else
	    parent->rb_left = left;
      }
      else
	 root = left;
      rb_set_parent(node, left);
   }
   
   void rbtree::rb_insert_color(struct rb_node *node)
   {
      struct rb_node *parent, *gparent;
      
      while ((parent = rb_parent(node)) && rb_is_red(parent))
      {
	 gparent = rb_parent(parent);
	 
	 if (parent == gparent->rb_left)
	 {
	    {
	       register struct rb_node *uncle = gparent->rb_right;
	       if (uncle && rb_is_red(uncle))
	       {
		  rb_set_black(uncle);
		  rb_set_black(parent);
		  rb_set_red(gparent);
		  node = gparent;
		  continue;
	       }
	    }
	    
	    if (parent->rb_right == node)
	    {
	       register struct rb_node *tmp;
	       __rb_rotate_left(parent);
	       tmp = parent;
	       parent = node;
	       node = tmp;
	    }
	    
	    rb_set_black(parent);
	    rb_set_red(gparent);
	    __rb_rotate_right(gparent);
	 } else {
	    {
	       register struct rb_node *uncle = gparent->rb_left;
	       if (uncle && rb_is_red(uncle))
	       {
		  rb_set_black(uncle);
		  rb_set_black(parent);
		  rb_set_red(gparent);
		  node = gparent;
		  continue;
	       }
	    }
	    
	    if (parent->rb_left == node)
	    {
	       register struct rb_node *tmp;
	       __rb_rotate_right(parent);
	       tmp = parent;
	       parent = node;
	       node = tmp;
	    }
	    
	    rb_set_black(parent);
	    rb_set_red(gparent);
	    __rb_rotate_left(gparent);
	 }
      }
      
      rb_set_black(root);
   }
   
   void rbtree::rb_erase_color(struct rb_node *node, struct rb_node *parent)
   {
      struct rb_node *other;
      
      while ((!node || rb_is_black(node)) && node != root)
      {
	 if (parent->rb_left == node)
	 {
	    other = parent->rb_right;
	    if (rb_is_red(other))
	    {
	       rb_set_black(other);
	       rb_set_red(parent);
	       __rb_rotate_left(parent);
	       other = parent->rb_right;
	    }
	    if ((!other->rb_left || rb_is_black(other->rb_left)) &&
		(!other->rb_right || rb_is_black(other->rb_right)))
	    {
	       rb_set_red(other);
	       node = parent;
	       parent = rb_parent(node);
	    }
	    else
	    {
	       if (!other->rb_right || rb_is_black(other->rb_right))
	       {
		  rb_set_black(other->rb_left);
		  rb_set_red(other);
		  __rb_rotate_right(other);
		  other = parent->rb_right;
	       }
	       rb_set_color(other, rb_color(parent));
	       rb_set_black(parent);
	       rb_set_black(other->rb_right);
	       __rb_rotate_left(parent);
	       node = root;
	       break;
	    }
	 }
	 else
	 {
	    other = parent->rb_left;
	    if (rb_is_red(other))
	    {
	       rb_set_black(other);
	       rb_set_red(parent);
	       __rb_rotate_right(parent);
	       other = parent->rb_left;
	    }
	    if ((!other->rb_left || rb_is_black(other->rb_left)) &&
		(!other->rb_right || rb_is_black(other->rb_right)))
	    {
	       rb_set_red(other);
	       node = parent;
	       parent = rb_parent(node);
	    }
	    else
	    {
	       if (!other->rb_left || rb_is_black(other->rb_left))
	       {
		  rb_set_black(other->rb_right);
		  rb_set_red(other);
		  __rb_rotate_left(other);
		  other = parent->rb_left;
	       }
	       rb_set_color(other, rb_color(parent));
	       rb_set_black(parent);
	       rb_set_black(other->rb_left);
	       __rb_rotate_right(parent);
	       node = root;
	       break;
	    }
	 }
      }
      if (node)
	 rb_set_black(node);
   }
   
   void rbtree::rb_erase(struct rb_node *node)
   {
      struct rb_node *child, *parent;
      int color;
      
      if (!node->rb_left)
	 child = node->rb_right;
      else if (!node->rb_right)
	 child = node->rb_left;
      else
      {
	 struct rb_node *old = node, *left;
	 
	 node = node->rb_right;
	 while ((left = node->rb_left) != NULL)
	    node = left;
	 
	 if (rb_parent(old)) {
	    if (rb_parent(old)->rb_left == old)
	       rb_parent(old)->rb_left = node;
	    else
	       rb_parent(old)->rb_right = node;
	 } else
	    root = node;
	 
	 child = node->rb_right;
	 parent = rb_parent(node);
	 color = rb_color(node);
	 
	 if (parent == old) {
	    parent = node;
	 } else {
	    if (child)
	       rb_set_parent(child, parent);
	    parent->rb_left = child;
	    
	    node->rb_right = old->rb_right;
	    rb_set_parent(old->rb_right, node);
	 }
	 
	 rb_set_parent(node, rb_parent(old));
	 rb_set_color(node, rb_color(old));
	 node->rb_left = old->rb_left;
	 rb_set_parent(old->rb_left, node);
	 
	 goto color;
      }
      
      parent = rb_parent(node);
      color = rb_color(node);
      
      if (child)
	 rb_set_parent(child, parent);
      if (parent)
      {
	 if (parent->rb_left == node)
	    parent->rb_left = child;
	 else
	    parent->rb_right = child;
      }
      else
	 root = child;
      
   color:
      if (color == RB_BLACK)
	 rb_erase_color(child, parent);
   }
   
   rb_node *rbtree::rb_first()
   {
      rb_node *n = root;
      
      if (!n)
	 return NULL;
      while (n->rb_left)
	 n = n->rb_left;
      return n;
   }
   
   rb_node *rbtree::rb_last()
   {
      struct rb_node *n = root;
      
      if (!n)
	 return NULL;
      while (n->rb_right)
	 n = n->rb_right;
      return n;
   }
   
   rb_node *rbtree::rb_next(const rb_node *node)
   {
      rb_node *parent;
      
      if(node->rb_parent == node)
	 return NULL;
      
      if(node->rb_right) 
      {
	 node = node->rb_right; 
	 while(node->rb_left)
	    node=node->rb_left;
	 return (struct rb_node *)node;
      }
      
      while((parent = node->rb_parent) && node == parent->rb_right)
	 node = parent;
      
      return parent;
   }
   
   
   void rbtree::__rb_print(rb_node *node)
   {
      if(!node)
	 return;
      
      if(node->rb_left)
	 __rb_print(node->rb_left);
      
      if(operation_mode == RBTREE_STR)
	 cout << "Value:" << node->key.str;
      else 
	 cout << "Value:" << node->key.num;
      if(rb_is_red(node))
	 cout << " Color: RED" << endl;
      else 
	 cout << " Color: BLACK" << endl;      

      if(node->rb_right)
	 __rb_print(node->rb_right);
      
   }
   
   void rbtree::print_tree()
   {
      __rb_print(root);
   }
};
