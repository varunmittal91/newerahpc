/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com> & Varun Dhawan <varundhawan5792@gmail.com>
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

#include <iostream>

#include <include/json.h>

using namespace std;

namespace neweraHPC
{
   nhpc_json_t::nhpc_json_t()
   {
      nodes = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      backtrack = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      search_queue = NULL;
      
      backtrack->insert(nodes);
   }
   
   nhpc_json_t::~nhpc_json_t()
   {
      
   }
   
   nhpc_status_t nhpc_json_t::add_element(int json_object, const char *key, const void *value)
   {
      rbtree_t *current = (rbtree_t *)backtrack->search(backtrack->ret_count());
      if(current == NULL)
      {
	 return NHPC_FAIL;
      }
      
      key_pair_t *key_pair = new key_pair_t;
      if(key)
	 nhpc_strcpy(&(key_pair->key), key);
      key_pair->json_object = json_object;

      if(json_object == JSON_STRING || json_object == JSON_NUMBER)
      {
	 if(value == NULL)
	 {
	    goto error_state;
	 }
	 
	 nhpc_strcpy((char **)&(key_pair->value), (char *)value);
      }
      else if(json_object == JSON_TRUE)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"true");	 
      }
      else if(json_object == JSON_FALSE)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"false");	 
      }
      else if(json_object == JSON_NULL)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"null");	 
      }      
      else if(json_object == JSON_ARRAY)
      {
	 rbtree_t *new_child = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 key_pair->value = new_child;
	 backtrack->insert(new_child);
      }
      else if(json_object == JSON_OBJECT)
      {
	 rbtree_t *new_child = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 key_pair->value = new_child;
	 backtrack->insert(new_child);
      }
      else 
      {
	 goto error_state;
      }
      
      current->insert(key_pair);
      return NHPC_SUCCESS;
      
   error_state:
      delete[] (key_pair->key);
      delete key_pair;
      return NHPC_FAIL;      
   }
   
   nhpc_status_t nhpc_json_t::close_element()
   {
      nhpc_status_t nrv = backtrack->erase(backtrack->ret_count());
      
      return nrv;
   }
   
   void nhpc_json_t::print()
   {
      key_pair_t *key_pair_tmp = (key_pair_t *)nodes->search(1);
      if(key_pair_tmp)
	 cout<<key_pair_tmp->key<<endl;
      
      if(backtrack->ret_count() != 0)
	 return;
      
      rbtree_t *queue = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      struct elem_t
      {
	 rbtree_t *branch;
	 int position;
      };
      
      elem_t *elem = new elem_t;
      elem->branch = nodes;
      elem->position = 1;
      queue->insert(elem);
      
      int tab_count = 0;

      cout << "{" <<endl;
      
      int i,j;
      key_pair_t *key_pair;
      
      while(queue->search(queue->ret_count()))
      {
	 for(j = 0; j < tab_count; j++)
	 {
	    cout<<"\t";
	 }

	 elem = (elem_t *)queue->search(queue->ret_count());
	 
	 key_pair_t *key_pair = (key_pair_t *)elem->branch->search(elem->position);
	 if(!key_pair)
	 {
	    queue->erase(queue->ret_count());
	    tab_count--;
	    cout<<endl;
	    continue;
	 }
	 else 
	 {
	    if(key_pair->key)
	       cout<<key_pair->key<<" : ";
	    if(key_pair->value)
	       cout<<(char *)(key_pair->value)<<endl;
	 }
	 if(key_pair->json_object == JSON_ARRAY || key_pair->json_object == JSON_OBJECT)
	 {
	    elem_t *elem_new = new elem_t;
	    elem_new->branch = (rbtree_t *)key_pair->value;
	    elem_new->position = 1;
	    queue->insert(elem_new);
	    tab_count++;
	 }
	 
	 elem->position++;
      }
      
      cout << "}" <<endl;
   }
   
   int nhpc_json_t::search(key_pair_t **_key_pair)
   {
      if(backtrack->ret_count() != 0)
      {
	 return JSON_INCOMPLETE;
      }
      
   loop:
      search_elem_t *search_elem;
      int search_queue_count = 1;
      
      if(search_queue == NULL)
      {
	 search_queue = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 search_elem = new search_elem_t;
	 search_elem->position = 1;
	 search_elem->branch = nodes;
	 search_queue->insert(search_elem);
      }
      else 
      {
	 search_queue_count = search_queue->ret_count();
	 
	 search_elem = (search_elem_t *)search_queue->search(search_queue_count);
	 if(search_elem == NULL)
	 {
	    delete search_queue;
	    return JSON_END;
	 }
      }
      
      key_pair_t *key_pair = (key_pair_t *)search_elem->branch->search(search_elem->position);
      if(!key_pair)
      {
	 search_queue->erase(search_queue_count);
	 delete search_elem;
	 goto loop;
      }
      else 
      {
	 (search_elem->position)++;
	 
	 if(key_pair->json_object == JSON_ARRAY || key_pair->json_object == JSON_OBJECT)
	 {
	    search_elem = new search_elem_t;
	    search_elem->branch = (rbtree_t *)key_pair->value;
	    search_elem->position = 1;
	    search_queue->insert(search_elem);
	 }  
	 
	 *(_key_pair) = key_pair;
	 return search_queue_count;
      }
   }
   
   void nhpc_json_t::print_new()
   {
      key_pair_t *key_pair;
      int status;
      status = search(&key_pair);
      
      while(status != JSON_END)
      {
	 cout<<"Level: "<<status<<endl;
	 if(key_pair->key)
	    cout<<key_pair->key<<endl;
	 if(key_pair->value)
	    cout<<(char *)key_pair->value<<endl;
	 cout<<endl;
	 
	 status = search(&key_pair);
      }
   }
};
