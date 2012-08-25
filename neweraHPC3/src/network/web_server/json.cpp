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
#include <string>

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
      rbtree_t *temp = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      temp->insert(nodes);
      
   loop:
      rbtree_t *current = (rbtree_t *)temp->search(1);
      if(current)
      {
	 for(int i = 1; i <= current->ret_count(); i++)
	 {
	    key_pair_t *key_pair = (key_pair_t *)current->search(i);
	    if(key_pair->key)
	       delete[] (key_pair->key);
	    
	    if(key_pair->json_object == JSON_OBJECT || key_pair->json_object == JSON_ARRAY)
	    {
	       temp->insert(key_pair->value);
	    }
	    else 
	    {
	       if(key_pair->value)
	       {
		  delete[] (char *)(key_pair->value);
	       }
	    }
	    
	    delete key_pair;
	 }
	 
	 delete current;
	 temp->erase(1);
	 
	 goto loop;
      }
      
      delete temp;
      delete backtrack;
      
      if(search_queue != NULL)
      {
	 search_elem_t *search_elem;
	 while(search_elem = (search_elem_t *)search_queue->search(1))
	 {
	    delete search_elem;
	 }
	 delete search_queue;
      }
   }
   
   nhpc_status_t nhpc_json_t::add_element(int json_object, const char *key, const void *value)
   {
      bool is_arrayobject = false;
      
      rbtree_t *current = (rbtree_t *)backtrack->search(backtrack->ret_count());
      if(current == NULL)
      {
	 return NHPC_FAIL;
      }
      
      key_pair_t *key_pair_last = (key_pair_t *)current->search(current->ret_count() - 1);
      if(key_pair_last)
	 if(key_pair_last->json_object != JSON_ARRAY && json_object == JSON_OBJECT && key == NULL)
	    return NHPC_FAIL;
      
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
	    search_queue = NULL;
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
   
   char *nhpc_json_t::get_stream()
   {
      string *out;
      int level;
      key_pair_t *key_pair;
      level = search(&key_pair);
      
      if(level == JSON_INCOMPLETE)
	 return NULL;
      
      out = new string;
      
      int old_level = 1;
      int last_object = 0;
      
      rbtree_t *stack = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      
      stream_elem_t *stream_elem = new stream_elem_t;
      stream_elem->json_object = JSON_OBJECT;
      stack->insert(stream_elem);
      (*out) += "{";
      
      bool is_branch = false;
      
      while(level != JSON_END)
      {
	 if(key_pair->key)
	    (*out) = (*out) + "\"" + key_pair->key + "\":";
	 
	 if(key_pair->value)
	 {
	    if(key_pair->json_object == JSON_STRING)
	       (*out) = (*out) + "\"" + (char *)(key_pair->value) + "\"";
	    else if((key_pair->json_object) != JSON_OBJECT && (key_pair->json_object) != JSON_ARRAY)
	       (*out) = (*out) + (char *)(key_pair->value);
	 }
	 
	 if((key_pair->json_object) == JSON_ARRAY || (key_pair->json_object) == JSON_OBJECT)
	 {
	    is_branch = true;
	    
	    if((key_pair->json_object) == JSON_ARRAY)
	       (*out) += "[";
	    else if((key_pair->json_object) == JSON_OBJECT)
	       (*out) += "{";
	    
	    stream_elem_t *stream_elem = new stream_elem_t;
	    stream_elem->json_object = key_pair->json_object;
	    stack->insert(stream_elem);
	 }
	 else 
	    is_branch = false;
	 
	 old_level = level;
	 last_object = key_pair->json_object;
	 level = search(&key_pair);
	 
	 if(old_level > level || (old_level == level && is_branch == true))
	 {
	    int count;
	    
	    if(old_level > level)
	       count = old_level - level;
	    else 
	       count = 1;
	    
	    for(int i = 0; i < count; i++)
	    {
	       stream_elem_t *stream_elem = (stream_elem_t *)stack->search(stack->ret_count());
	       if(stream_elem->json_object == JSON_ARRAY)
	       {
		  (*out) += "]";
	       }
	       else
	       {
		  (*out) += "}";
	       }
	       
	       delete stream_elem;
	       stack->erase(stack->ret_count());
	    }
	    
	    is_branch = false;
	 }
	 
	 if(old_level >= level && level != JSON_END)
	 {
	    (*out) += ",";
	 }
      }
      
      if(stack->ret_count() != 0)
      {
	 stream_elem_t *stream_elem = (stream_elem_t *)stack->search(stack->ret_count());
	 delete stream_elem;
	 
	 (*out) += '}';
      }
      
      delete stack;
      
      char *out_str = (char *)(*out).c_str();
      delete out;
      
      return out_str;
   }
   
   bool nhpc_json_t::is_delimiter(char in_char)
   {
      if(in_char == ',' || in_char == '}' || in_char == ']')
	 return true;
      
      return false;
   }
   
   nhpc_status_t nhpc_json_t::build_structure(const char *json_string)
   {
      char *json_string_ptr = (char *)json_string;
      char *key = NULL;
      char *value = NULL;
      int start_pos, end_pos;
      
      start_pos = 1 + skip_white_spaces(&json_string_ptr);
      
      if(*json_string_ptr != '{')
	 goto error_state;
      json_string_ptr++;
      start_pos++;
      end_pos = start_pos;
      
      while((*json_string_ptr) != '\0')
      {
	 char **key_value_str;
	 if(key == NULL)
	    key_value_str = &key;
	 else 
	    key_value_str = &value;
	 
	 start_pos += skip_white_spaces((char **)&json_string_ptr);
	 end_pos = start_pos;
	 
	 if(*json_string_ptr == '"')
	 {
	    start_pos++;
	    json_string_ptr++;
	    
	    while(*json_string_ptr != '"')
	    {
	       if(*json_string_ptr == '\0')
		  goto error_state;
	       
	       json_string_ptr++;
	       end_pos++;
	    }
	    
	    *key_value_str = nhpc_substr(json_string, start_pos, end_pos);
	    start_pos = end_pos + 1;
	    json_string_ptr++;
	    
	    start_pos += skip_white_spaces(&json_string_ptr);
	    
	    if(*json_string_ptr != ':')
	    {
	       goto error_state;	       
	    }
	    
	    start_pos++;
	    json_string_ptr++;
	    if(*json_string_ptr == '\0')
	    {
	       goto error_state;
	    }
	    
	    start_pos += skip_white_spaces(&json_string_ptr);
	    end_pos = start_pos;
	    
	    if(*json_string_ptr == '{' || *json_string_ptr == '[')
	    {
	       if(*json_string_ptr == '[')
		  add_element(JSON_ARRAY, key);
	       else 
		  add_element(JSON_OBJECT, key);		  
	       delete[] key;
	       key = NULL;
	       
	       start_pos += 2;
	       json_string_ptr++;
	    }
	    else 
	    {
	       bool ignore_space = false;
	       if(*json_string_ptr == '"')
		  ignore_space = true;
		  
	       while(!is_delimiter(*json_string_ptr))
	       {
		  if(!ignore_space)
		     if(*json_string_ptr == ' ')
			break;
		  if(*json_string_ptr == '\0')
		  {		  
		     goto error_state;
		  }
		  
		  end_pos++;
		  *json_string_ptr++;

		  if(*json_string_ptr == '"')
		  {
		     ignore_space = false;
		  }
	       }
	       
	       value = nhpc_substr(json_string, start_pos, end_pos);
	       start_pos = end_pos + 1;
	       
	       if(nhpc_strcmp(value, "true") == NHPC_SUCCESS)
	       {
		  add_element(JSON_TRUE, key);
	       }
	       else if(nhpc_strcmp(value, "false") == NHPC_SUCCESS)
	       {
		  add_element(JSON_FALSE, key);
	       }
	       else if(nhpc_strcmp(value, "null") == NHPC_SUCCESS)
	       {
		  add_element(JSON_NULL, key);
	       }
	       else if(nhpc_strcmp(value, "\"*\"") == NHPC_SUCCESS)
	       {
		  add_element(JSON_STRING, key, value);
	       }
	       else 
	       {
		  add_element(JSON_NUMBER, key, value);
	       }
	       
	       delete[] key;
	       delete[] value;
	       key = NULL;
	       value = NULL;
	    }
	 }
	 else if(*json_string_ptr == '{')
	 {
	    add_element(JSON_OBJECT);
	    
	    json_string_ptr++;
	    start_pos++;
	 }
	 else if(is_delimiter(*json_string_ptr))
	 {
	    if(*json_string_ptr != ',')
	       close_element();

	    json_string_ptr++;
	    start_pos++;
	 }
	 else 
	 {
	    json_string_ptr++;
	    start_pos++;
	 }
      }
      
      skip_white_spaces(&json_string_ptr);
      
      if(*json_string_ptr == '}')
	 close_element();
      
      if(backtrack->ret_count() != 0)
      {
	 goto error_state;
      }
      
      return NHPC_SUCCESS;
      
   error_state:
      return NHPC_FAIL;
   }
};