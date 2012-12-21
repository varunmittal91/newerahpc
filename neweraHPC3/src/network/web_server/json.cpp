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
#include <include/error.h>

using namespace std;

namespace neweraHPC
{
   const char* JSON_OBJECT_STRINGS[5] = {"{JSON_INCOMPLETE}", "{JSON_END}", "{JSON_ARRAY}", "{JSON_OBJECT}", "{JSON_OBJECT_NOT_FOUND}"};
   
   json_t::json_t()
   {
      root = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      
      key_pair_t *key_pair = new key_pair_t;
      memset(key_pair, 0, sizeof(key_pair_t));

      key_pair->branch        = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      key_pair->branch_object = new rbtree_t(NHPC_RBTREE_STR);
      key_pair->json_object   = JSON_OBJECT;

      root_key_pair = key_pair;
      root->insert(key_pair);
      
      search_queue     = NULL;
      current_key_pair = NULL;
      
      final_json_length = 2;
      final_json_string = NULL;
   }
   
   json_t::~json_t()
   {
      reinitialize();
   }
   
   void json_t::initialize()
   {
      root = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      
      key_pair_t *key_pair = new key_pair_t;
      memset(key_pair, 0, sizeof(key_pair_t));
      
      key_pair->branch        = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      key_pair->branch_object = new rbtree_t(NHPC_RBTREE_STR);
      key_pair->json_object   = JSON_OBJECT;
      
      root_key_pair = key_pair;
      root->insert(key_pair);
      
      search_queue     = NULL;
      current_key_pair = NULL;
      
      final_json_length = 2;
      final_json_string = NULL;      
   }
   
   void json_t::reinitialize()
   {
      if(final_json_string)
	 delete[] final_json_string;
      
      if(search_queue)
      {
	 search_elem_t *search_elem;
	 int search_queue_count = search_queue->ret_count();
	 
	 for(int i = 1; i <= search_queue_count; i++)
	 {
	    search_elem = (search_elem_t *)search_queue->search(i);
	    delete search_elem;
	 }
	 
	 delete search_queue;
	 search_queue = NULL;
      }
      
      if(root_key_pair)
      {
	 key_pair_t *key_pair, *key_pair2;

	 rbtree_t *backtrack = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 backtrack->insert(root_key_pair);
	 rbtree_t *temp;
	 
	 while(backtrack->ret_count() > 0)
	 {
	    key_pair = (key_pair_t *)backtrack->search(1);
	    
	    rbtree_t *temp = key_pair->branch;
	    
	    for(int i = 1; i <= temp->ret_count(); i++)
	    {
	       key_pair2 = (key_pair_t *)temp->search(i);
	       
	       if(key_pair2->json_object == JSON_ARRAY || key_pair2->json_object == JSON_OBJECT)
	       {
		  backtrack->insert(key_pair2);
	       }
	       else 
	       {
		  if(key_pair2->key)
		     delete[] key_pair2->key;
		  if(key_pair2->value)
		     delete[] key_pair2->value;
		  
		  delete key_pair2;
	       }
	    }
	    
	    delete key_pair->branch;

	    if(key_pair->branch_object)
	    {
	       delete key_pair->branch_object;
	    }
	    
	    if(key_pair->key)
	       delete[] key_pair->key;
	    if(key_pair->value)
	       delete[] key_pair->value;
	    
	    delete key_pair;
	    
	    backtrack->erase(1);
	 }
	 
	 delete backtrack;
	 delete root;
	 root_key_pair = NULL;
	 root = NULL;
      }
   }
   
   int json_t::traverse(key_pair_t **_key_pair)
   {
      int level = root->ret_count();
      if(level > 0)
      {
	 cout << "json not complete" << endl;
	 return JSON_INCOMPLETE;
      }
       
   loop:
      search_elem_t *search_elem;
      int search_queue_count = 1;
      
      if(search_queue == NULL)
      {
	 search_queue = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 search_elem = new search_elem_t;
	 memset(search_elem, 0, sizeof(search_elem_t));
	 
	 search_elem->position = 1;
	 search_elem->branch = root_key_pair->branch;
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
	    memset(search_elem, 0, sizeof(search_elem_t));
	    
	    search_elem->branch = key_pair->branch;
	    search_elem->position = 1;
	    search_queue->insert(search_elem);
	 }  
	 *_key_pair = key_pair;
	 
	 return key_pair->json_object;
      }
   }
   
   const char *json_t::get_string()
   {
      int level = root->ret_count();
      if(level > 0)
	 return NULL;
      
      if(final_json_string)
	 return final_json_string;
      else 
	 final_json_string = new char [final_json_length + 1];
      
      char *tmp_json_string = final_json_string;
      
      int json_status;
      int current_position, element_count;
      
      key_pair_t *key_pair;
      search_elem_t *search_elem, *search_elem_old;
      
      *tmp_json_string = '{';
      tmp_json_string++;
      
      while((json_status = traverse(&key_pair)) != JSON_END)
      {
	 search_elem      = (search_elem_t *)search_queue->search(search_queue->ret_count());
	 search_elem_old  = (search_elem_t *)search_queue->search(search_queue->ret_count() - 1);
	 current_position = (search_elem->position);
	 element_count    = search_elem->branch->ret_count();
	 
	 if(key_pair->json_object == JSON_OBJECT_CLOSE || key_pair->json_object == JSON_ARRAY_CLOSE)
	 {
	    if(key_pair->json_object == JSON_OBJECT_CLOSE)
	    {
	       *tmp_json_string = '}';
	    }
	    else if(key_pair->json_object == JSON_ARRAY_CLOSE)
	    {
	       *tmp_json_string = ']';
	    }
	    tmp_json_string++;
	    
	    if(search_elem_old)
	    {
	       current_position = search_elem_old->position;
	       element_count    = search_elem_old->branch->ret_count();
	       
	       if(current_position < element_count)
	       {
		  *tmp_json_string = ',';
		  tmp_json_string++;
	       }
	    }
	    
	    continue;
	 }
	 
	 if(key_pair->key)
	 {
	    nhpc_size_t key_len = strlen(key_pair->key);
	    memcpy(tmp_json_string, key_pair->key, key_len);
	    tmp_json_string += key_len;
	    
	    memcpy(tmp_json_string, ": ", 2);
	    tmp_json_string += 2;
	 }
	 if(key_pair->value)
	 {
	    nhpc_size_t value_len = strlen(key_pair->value);
	    
	    if(key_pair->json_object == JSON_STRING)
	    {
	       *tmp_json_string = '"';
	       memcpy((tmp_json_string + 1), key_pair->value, value_len);
	       tmp_json_string += (value_len + 1);
	       *tmp_json_string = '"';
	       tmp_json_string++;
	    }
	    else 
	    {
	       memcpy(tmp_json_string, key_pair->value, value_len);
	       tmp_json_string += value_len;
	    }
	 }
	 if(key_pair->json_object == JSON_ARRAY || key_pair->json_object == JSON_OBJECT)
	 {
	    if(key_pair->json_object == JSON_ARRAY)
	    {
	       *tmp_json_string = '[';
	    }
	    if(key_pair->json_object == JSON_OBJECT)
	    {
	       *tmp_json_string = '{';
	    }
	    
	    tmp_json_string++;
	 }
	 
	 if(current_position < element_count && current_position > 1)
	 {
	    *tmp_json_string = ',';
	    tmp_json_string++;
	 }
      }
      
      *tmp_json_string = '\0';
      return final_json_string;
   }

   
   void json_t::print()
   {
      int level = root->ret_count();
      if(level > 0)
      {
	 cout << JSON_OBJECT_STRINGS[JSON_INCOMPLETE] << endl;
	 return;
      }
      
      key_pair_t *key_pair;
      search_elem_t *search_elem, *search_elem_old;

      int json_status;
      int current_position, element_count;
      
      cout << "{";
      
      while((json_status = traverse(&key_pair)) != JSON_END)
      {
	 search_elem      = (search_elem_t *)search_queue->search(search_queue->ret_count());
	 search_elem_old  = (search_elem_t *)search_queue->search(search_queue->ret_count() - 1);
	 current_position = (search_elem->position);
	 element_count    = search_elem->branch->ret_count();
	 
	 if(key_pair->json_object == JSON_OBJECT_CLOSE || key_pair->json_object == JSON_ARRAY_CLOSE)
	 {
	    if(key_pair->json_object == JSON_OBJECT_CLOSE)
	    {
	       cout << "}" ;
	    }
	    else if(key_pair->json_object == JSON_ARRAY_CLOSE)
	    {
	       cout << "]";
	    }
	    
	    if(search_elem_old)
	    {
	       current_position = search_elem_old->position;
	       element_count    = search_elem_old->branch->ret_count();
	    
	       if(current_position < element_count)
		  cout << ",";
	    }

	    continue;
	 }
	 
	 if(key_pair->key)
	    cout << key_pair->key << ": ";
	 if(key_pair->value)
	 {
	    if(key_pair->json_object == JSON_STRING)
	       cout << '"' <<key_pair->value << '"';
	    else 
	       cout << key_pair->value;
	 }
	 if(key_pair->json_object == JSON_ARRAY || key_pair->json_object == JSON_OBJECT)
	 {
	    if(key_pair->json_object == JSON_ARRAY)
	       cout << "[";
	    if(key_pair->json_object == JSON_OBJECT)
	       cout << "{";
	 }
	 
	 if(current_position < element_count && current_position > 1)
	    cout << ",";
      }
      
      cout << endl;
      cout << "JSON length: " << final_json_length << endl;
   }
   
   json_t::key_pair_t *json_t::search(int json_object, int *key_num, const char *key_str, const char **response)
   {
      /*
      int level = root->ret_count();
      if(level > 0)
      {
	 *response = JSON_OBJECT_STRINGS[JSON_INCOMPLETE];
	 return NULL;
      }
       */
      
      *response = NULL;
      
      key_pair_t *key_pair;
      int        *json_status;
      
      if(current_key_pair == NULL)
	 current_key_pair = root_key_pair;

      json_status = &(current_key_pair->json_object);
      
      if(*json_status != json_object)
      {
	 *response = JSON_OBJECT_STRINGS[*json_status];
	 return NULL;
      }
      
      if(json_object == JSON_ARRAY)
      {
	 key_pair = (key_pair_t *)current_key_pair->branch->search(*key_num);
      }
      else 
      {
	 key_pair = (key_pair_t *)current_key_pair->branch_object->search(key_str);
      }
      
      if(key_pair)
      {
	 json_status = &(key_pair->json_object);
	 
	 if(*json_status == JSON_OBJECT || *json_status == JSON_ARRAY)
	 {
	    current_key_pair = key_pair;
	    *response = JSON_OBJECT_STRINGS[*json_status];
	    return current_key_pair;
	 }
	 
	 return key_pair;
	 *response = NULL;
      }
      else 
      {
	 *response = JSON_OBJECT_STRINGS[JSON_OBJECT_NOT_FOUND];
	 return NULL;
      }
   }
   
   const char *json_t::operator[](const char *key)
   {
      key_pair_t *key_pair;
      const char *response;
      
      key_pair = search(JSON_OBJECT, NULL, key, &response);
      
      if(response)
      {
	 return response;
      }
      else 
      {
	 return key_pair->value;
      }
   }
   
   const char *json_t::operator[](int key)
   {
      key_pair_t *key_pair;
      const char *response;
      
      key_pair = search(JSON_ARRAY, &key, NULL, &response);

      if(response)
      {
	 return response;
      }
      else 
      {
	 return key_pair->value;
      }
   }
   
   int json_t::count()
   {
      if(current_key_pair == NULL)
	 current_key_pair = root_key_pair;
      
      if(current_key_pair->json_object == JSON_OBJECT)
	 return current_key_pair->branch->ret_count();
      else 
	 return (current_key_pair->branch->ret_count() - 1);
   }
   
   nhpc_status_t json_t::update_value(key_pair_t *key_pair, const char *input)
   {
      int *json_object = &(key_pair->json_object);
      
      nhpc_size_t len_original = strlen(key_pair->value);
      nhpc_size_t len_new      = strlen(input);
      
      if(final_json_string)
      {
	 delete[] final_json_string;
	 final_json_string = NULL;
      }
      
      if(len_new != len_original)
	 final_json_length += (len_new - len_original);
      
      delete[] (key_pair->value);
      nhpc_strcpy(&(key_pair->value), input);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t json_t::update(int key, const char *input)
   {
      const char    *response;
      key_pair_t    *key_pair;
      nhpc_status_t nrv; 
      
      key_pair = search(JSON_ARRAY, &key, NULL, &response);
      
      if(response)
	 return NHPC_FAIL;
      else 
	 nrv = update_value(key_pair, input);
      
      return nrv;
   }

   nhpc_status_t json_t::update(const char *key, const char *input)
   {
      const char    *response;
      key_pair_t    *key_pair;
      nhpc_status_t nrv; 
      
      key_pair = search(JSON_OBJECT, NULL, key, &response);      
      
      if(response)
	 return NHPC_FAIL;
      else 
	 nrv = update_value(key_pair, input);
      
      return nrv;
   }
   
   void json_t::reinitialize_search()
   {
      current_key_pair = NULL;
   }
   
   nhpc_status_t json_t::close_element()
   {
      int level = root->ret_count();
      if(level == 0)
	 return NHPC_FAIL;

      key_pair_t *last_key_pair = (key_pair_t *)(*root).search(level);
      
      key_pair_t *key_pair = new key_pair_t;
      memset(key_pair, 0, sizeof(key_pair_t));

      if(last_key_pair->json_object == JSON_OBJECT)
      {
	 key_pair->json_object = JSON_OBJECT_CLOSE;
      }
      else if(last_key_pair->json_object == JSON_ARRAY)
      {
	 key_pair->json_object = JSON_ARRAY_CLOSE;
      }
      else 
	 LOG_ERROR("Invalid close");
      
      last_key_pair->branch->insert(key_pair);
      
      root->erase(level);
      
      return NHPC_SUCCESS;
   }
   
   nhpc_status_t json_t::add_element(int json_object, const char *input1, const char *input2)
   {
      int level = root->ret_count();
      if(level == 0)
	 return NHPC_FAIL;

      key_pair_t *last_key = (key_pair_t *)(*root).search(level);
      int current_json_object = last_key->json_object;
      
      const char *key = NULL, *value = NULL;
      
      bool copy_key = false;
      bool copy_value = false;
      bool copy_value_quotes = false;
      bool create_object = false;
      bool is_valid = false;
      
      if(current_json_object == JSON_ARRAY)
      {
	 if(json_object == JSON_ARRAY || json_object == JSON_OBJECT)
	 {
	    if(input1 || input2)
	       return NHPC_FAIL;
	    
	    create_object = true;
	 }
	 else if(json_object == JSON_STRING || json_object == JSON_NUMBER)
	 {
	    if(!input1 || input2)
	       return NHPC_FAIL;
	    
	    if(json_object == JSON_STRING)
	       copy_value_quotes = true;
	    copy_value = true;
	    
	    value = input1;
	 }
	 else 
	 {
	    if(json_object == JSON_TRUE)
	    {
	       value = "true";
	    }
	    else if(json_object == JSON_FALSE)
	    {
	       value = "false";
	    }
	    else if(json_object == JSON_NULL)
	    {
	       value = "null";
	    }
	    
	    copy_value = true;
	 }
	 
	 is_valid = true;
      }
      else if(json_object == JSON_STRING || json_object == JSON_NUMBER)
      {
	 key = input1, value = input2;
	 
	 if(!key || !value)
	    return NHPC_FAIL;
	 
	 if(json_object == JSON_STRING)
	    copy_value_quotes = true;
	 copy_key = true;
	 copy_value = true;
	 
	 is_valid = true;
      }
      else if(json_object == JSON_TRUE || json_object == JSON_FALSE || json_object == JSON_NULL || json_object == JSON_OBJECT || json_object == JSON_ARRAY)
      {
	 if(input2 || !input1)
	    return NHPC_FAIL;
	 
	 key = input1;
	 
	 if(json_object != JSON_ARRAY && json_object != JSON_OBJECT)
	 {
	    if(json_object == JSON_TRUE)
	       value = "true";
	    else if(json_object == JSON_FALSE)
	       value = "false";
	    else if(json_object == JSON_NULL)
	       value = "null";
	    
	    copy_value = true;
	 }
	 else 
	    create_object = true;
	 
	 copy_key = true;
	 
	 is_valid = true;
      }
      
      if(!is_valid)
	 return NHPC_FAIL;
      
      key_pair_t *key_pair = new key_pair_t;
      memset(key_pair, 0, sizeof(key_pair_t));
      key_pair->json_object = json_object;
      
      if(copy_key)
      {
	 nhpc_size_t len = strlen(key) + 3;
	 
	 char *key_str = new char [len];
	 *key_str = '"';
	 memcpy((key_str + 1), key, len - 3);
	 key_str[len - 2] = '"';
	 key_str[len - 1] = '\0';
	 key_pair->key = key_str;
	 
	 final_json_length += len - 1;
      }
      if(copy_value)
      {
	 nhpc_strcpy(&(key_pair->value), value);  
	 final_json_length += strlen(value);
	 
	 if(copy_key)
	    final_json_length += 2;
	 if(copy_value_quotes)
	    final_json_length += 2;
      }
      
      if(create_object)
      {
	 key_pair->branch = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 if(json_object == JSON_OBJECT)
	    key_pair->branch_object = new rbtree_t(NHPC_RBTREE_STR);
	 
	 root->insert(key_pair);
	 
	 final_json_length += 2;
	 if(copy_key)
	    final_json_length += 2;
      }
      last_key->branch->insert(key_pair);
      if(last_key->json_object == JSON_OBJECT)
      {
	 last_key->branch_object->insert(key_pair, key);
      }
      
      if(last_key->branch->ret_count() > 1)
      {
	 final_json_length += 1;
      }
      
      return NHPC_SUCCESS;
   }  
   
   nhpc_status_t json_t::build_structure(const char *json_string)
   {
      char *json_string_ptr = (char *)json_string;
      char *key             = NULL;
      char *value           = NULL;

      int start_pos, end_pos;
      int object_type;
      
      key_pair_t *last_key = (key_pair_t *)(*root).search(1);
      
      skip_white_spaces(&json_string_ptr);
      if(*json_string_ptr != '{')
	 goto error_state;
      json_string_ptr++;
      
      while(*json_string_ptr)
      {
	 skip_white_spaces(&json_string_ptr);

	 switch(*json_string_ptr)
	 {
	    case '[':
	    case '{':
	    {
	       object_type = (*json_string_ptr == '{') ? JSON_OBJECT : JSON_ARRAY;
	       
	       if(object_type == JSON_ARRAY)
	       {
		  if(!key && object_type == JSON_ARRAY)
		     goto error_state;
		  
		  add_element(JSON_ARRAY, key);
		  
		  delete[] key;
		  key = NULL;
	       }
	       else 
	       {
		  if(key)
		  {
		     add_element(JSON_OBJECT, key);
		     
		     delete[] key;
		     key = NULL;
		  }
		  else 
		     add_element(JSON_OBJECT);
	       }
	       
	       last_key = (key_pair_t *)(*root).search(root->ret_count());
	       
	       json_string_ptr++;
	    }
	       break;
	       
	    case '}':
	    case ']':
	    {
	       close_element();
	       last_key = (key_pair_t *)(*root).search(root->ret_count());
	       
	       json_string_ptr++;
	    }
	       break;
	       
	    case ':':
	    {
	       if(!key)
		  goto error_state;
	       json_string_ptr++;
	    }
	       break;
	       
	    case ',':
	    {
	       json_string_ptr++;
	    }
	       break;
	       
	    case '"':
	    {
	       start_pos = 2;
	       end_pos = nhpc_strfind(json_string_ptr, '"', start_pos);
	       
	       char *string = nhpc_substr(json_string_ptr, start_pos, end_pos - 1);
	       if(!key)
	       {
		  if(last_key->json_object == JSON_ARRAY)
		  {
		     add_element(JSON_STRING, string);
		     
		     delete[] string;
		  }
		  else 
		  {
		     key = string;
		  }
	       }
	       else 
	       {
		  add_element(JSON_STRING, key, string);
		  delete[] key;
		  delete[] string;
		  
		  key    = NULL;
	       }
	       
	       json_string_ptr += (end_pos);
	    }
	       break;
	       
	    case 'n':
	    case 't':
	    case 'f':
	    {
	       const char *value_str;
	       int increment_factor;
	       
	       if(nhpc_strcmp(json_string_ptr, "null*") == NHPC_SUCCESS)
	       {
		  value_str = "null";
		  object_type = JSON_NULL;

		  increment_factor = 4;
	       }
	       else if(nhpc_strcmp(json_string_ptr, "true*") == NHPC_SUCCESS)
	       {
		  value_str = "true";
		  object_type = JSON_TRUE;
		  
		  increment_factor = 4;
	       }
	       else if(nhpc_strcmp(json_string_ptr, "false*") == NHPC_SUCCESS)
	       {
		  value_str = "false";
		  object_type = JSON_FALSE;
		  
		  increment_factor = 5;
	       }
	       else 
	       {
		  goto error_state;
	       }
	       
	       if(key && last_key->json_object != JSON_ARRAY)
	       {
		  add_element(object_type, key);
		  delete[] key;
		  key = NULL;
	       }
	       else if(key && last_key->json_object == JSON_ARRAY) 
	       {
		  goto error_state;
	       }
	       else 
	       {
		  add_element(object_type);
	       }
	       
	       json_string_ptr += increment_factor;
	    }
	       break;
	       
	    case '-':
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	    {
	       int digits = 0;
	       
	       while(*json_string_ptr)
	       {
		  if(((int)(*json_string_ptr) >= 48 && (int)(*json_string_ptr) <= 57) || (int)(*json_string_ptr) == 45)
		  {
		     json_string_ptr++;
		     digits++;
		  }
		  else 
		     break;
	       }
	       
	       json_string_ptr -= digits;

	       value = nhpc_substr(json_string_ptr, 1, digits);
	       
	       json_string_ptr += digits;
	       
	       if(key && last_key->json_object == JSON_ARRAY)
		  goto error_state;
	       else if(key && last_key->json_object == JSON_OBJECT)
	       {
		  add_element(JSON_NUMBER, key, value);
		  delete[] key;
		  delete[] value;
		  key   = NULL;
		  value = NULL;
	       }
	       else 
	       {
		  add_element(JSON_NUMBER, value);
		  delete[] value;
		  value = NULL;
	       }
	       
	    }
	       break;
	    default:
	       goto error_state;
	 }
      }
      
      if((*root).ret_count() >0)
	 goto error_state;
      
      return NHPC_SUCCESS;

   error_state:
      
      if(key)
	 delete[] key;
      if(value)
	 delete[] value;
      
      reinitialize();
      initialize();
      
      return NHPC_FAIL;
   }
   
   
   bool json_t::is_delimiter(char in_char)
   {
      if(in_char == ',' || in_char == '}' || in_char == ']' || in_char == '\0')
	 return true;
      
      return false;
   }
   
   nhpc_json_t::nhpc_json_t()
   {
      current_level = 1;
      
      stream_length = 2;
      
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
	 search_queue = NULL;
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
      
      key_pair_t *key_pair_last = (key_pair_t *)current->search(current->ret_count());
       
      int stream_length_new = stream_length;
      if(current->ret_count() > 0)
	 stream_length_new += 1;
      
      key_pair_t *key_pair = new key_pair_t;
      memset(key_pair, 0, sizeof(key_pair_t));
	     
      if(key)
      {
	 nhpc_strcpy(&(key_pair->key), key);
	 
	 stream_length_new += strlen(key_pair->key) + 4;
      }
      key_pair->json_object = json_object;
      key_pair->level = current_level;
      
      if(json_object == JSON_STRING || json_object == JSON_NUMBER)
      {
	 if(value == NULL)
	 {
	    goto error_state;
	 }
	 
	 nhpc_strcpy((char **)&(key_pair->value), (char *)value);
	 
	 if(json_object == JSON_STRING)
	    stream_length_new += strlen((char *)key_pair->value) + 2;
	 else 
	    stream_length_new += strlen((char *)key_pair->value);
      }
      else if(json_object == JSON_TRUE)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"true");
	 stream_length_new += 4;
      }
      else if(json_object == JSON_FALSE)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"false");	 
	 stream_length_new += 5;
      }
      else if(json_object == JSON_NULL)
      {
	 nhpc_strcpy((char **)&(key_pair->value), (char *)"null");	 
	 stream_length_new += 4;
      }      
      else if(json_object == JSON_ARRAY)
      {
	 rbtree_t *new_child = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 key_pair->value = new_child;
	 backtrack->insert(new_child);
	 stream_length_new += 2;
	 
	 current_level++;
      }
      else if(json_object == JSON_OBJECT)
      {
	 rbtree_t *new_child = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
	 key_pair->value = new_child;
	 backtrack->insert(new_child);
	 stream_length_new += 2;
	 
	 current_level++;
      }
      else 
      {
	 goto error_state;
      }
      
      stream_length = stream_length_new;
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
      if(nrv == NHPC_SUCCESS)
      {
	 current_level--;
      }
      
      return nrv;
   }
      
   int nhpc_json_t::search(key_pair_t **_key_pair, int *child_count)
   {
      *_key_pair = NULL;
      
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
	 memset(search_elem, 0, sizeof(search_elem_t));
	 
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
	    memset(search_elem, 0, sizeof(search_elem_t));
	    
	    search_elem->branch = (rbtree_t *)key_pair->value;
	    search_elem->position = 1;
	    search_queue->insert(search_elem);

	    if(child_count)
	       *child_count = search_elem->branch->ret_count();
	 }  
	 
	 *(_key_pair) = key_pair;
	 return search_queue_count;
      }
   }
   
   const char *nhpc_json_t::get_stream()
   {
      if(backtrack->ret_count() > 0)
	 return NULL;
      
      struct object_status_t
      {
	 rbtree_t *tree;
	 int elements_done;
	 int elements;
	 int json_object;
      };
      rbtree_t *stack = new rbtree_t(NHPC_RBTREE_NUM_MANAGED);
      
      key_pair_t *key_pair = NULL;
      key_pair_t *key_pair_current;
      
      int json_object = 0;
      int json_object_old = 0;      
      int tab_count = 0;
      
      int child_count;
      int child_count_tmp = 0;
      
      object_status_t *object_status = new object_status_t;
      memset(object_status, 0, sizeof(object_status_t));
      
      object_status->tree = nodes;
      object_status->elements_done = 0;
      object_status->elements = nodes->ret_count();
      object_status->json_object = JSON_OBJECT;
      stack->insert(object_status);
      
      char *json_out_str = new char [stream_length + 1];
      memset(json_out_str, 0, stream_length + 1);
      json_out_str[stream_length] = '\0';
      char *json_out_str_tmp = json_out_str;
      
      *json_out_str_tmp = '{';
      json_out_str_tmp++;
      
      do 
      {
	 object_status_t *object_status_current = (object_status_t *)stack->search(stack->ret_count());
	 
	 if(key_pair)
	 {
	    if(object_status_current)
	       (object_status_current->elements_done) += 1;
	    
	    if(key_pair->json_object == JSON_OBJECT || key_pair->json_object == JSON_ARRAY)
	    {	       
	       object_status_t *object_status = new object_status_t;
	       memset(object_status, 0, sizeof(object_status_t));
	       
	       object_status->tree = (rbtree_t *)(key_pair->value);
	       object_status->elements_done = 0;
	       object_status->elements = ((rbtree_t *)(key_pair->value))->ret_count();
	       object_status->json_object = key_pair->json_object;
	       stack->insert(object_status);
	       
	       if(key_pair->json_object == JSON_OBJECT)
	       {
		  if(object_status_current && object_status_current->json_object == JSON_ARRAY)
		  {
		     *json_out_str_tmp = '{';
		     json_out_str_tmp++;
		  }
		  else 
		  {
		     *json_out_str_tmp = '"';
		     json_out_str_tmp++;
		     memcpy((json_out_str_tmp), key_pair->key, strlen(key_pair->key));
		     json_out_str_tmp = json_out_str_tmp + strlen(key_pair->key);		     
		     memcpy((json_out_str_tmp), "\": {", 4);
		     json_out_str_tmp += 4;
		  }
	       }
	       else 
	       {
		  *json_out_str_tmp = '"';
		  json_out_str_tmp++;
		  memcpy((json_out_str_tmp), key_pair->key, strlen(key_pair->key));
		  json_out_str_tmp = json_out_str_tmp + strlen(key_pair->key);
		  memcpy((json_out_str_tmp), "\": [", 4);
		  json_out_str_tmp += 4;
	       }
	       
	       object_status_current = object_status;
	    }
	    else 
	    {
	       *json_out_str_tmp = '"'; 
	       json_out_str_tmp++;
	       memcpy((json_out_str_tmp), key_pair->key, strlen(key_pair->key));
	       json_out_str_tmp = json_out_str_tmp + strlen(key_pair->key);
	       memcpy((json_out_str_tmp), "\"", 1);
	       json_out_str_tmp++;
	       
	       if(key_pair->json_object == JSON_NULL)
	       {
		  memcpy((json_out_str_tmp), ": null", 6);
		  json_out_str_tmp += 6;
	       }
	       else if(key_pair->json_object == JSON_TRUE)
	       {
		  memcpy((json_out_str_tmp), ": true", 6);
		  json_out_str_tmp += 6;
	       }
	       else if(key_pair->json_object == JSON_FALSE)
	       {
		  memcpy((json_out_str_tmp), ": false", 7);
		  json_out_str_tmp += 7;
	       }
	       else if(key_pair->json_object == JSON_NUMBER)
	       {
		  memcpy((json_out_str_tmp), ": ", 2);
		  json_out_str_tmp += 2;
		  memcpy((json_out_str_tmp), (const char *)key_pair->value, strlen((char *)key_pair->value));
		  json_out_str_tmp += strlen((char *)key_pair->value);
	       }
	       else
	       {
		  memcpy((json_out_str_tmp), ": ", 2);
		  json_out_str_tmp += 2;
		  
		  *json_out_str_tmp = '"';
		  json_out_str_tmp++;
		  memcpy((json_out_str_tmp), (const char *)key_pair->value, strlen((char *)key_pair->value));
		  json_out_str_tmp += strlen((char *)key_pair->value);		  
		  *json_out_str_tmp = '"';
		  json_out_str_tmp++;
	       }
	       
	       if(object_status_current && object_status_current->elements_done < object_status_current->elements)
	       {
		  *json_out_str_tmp = ',';
		  json_out_str_tmp++;
	       }
	    }	    
	    
	    while(object_status_current && object_status_current->elements_done == object_status_current->elements)
	    {
	       stack->erase(stack->ret_count());
	       
	       if(object_status_current->json_object == JSON_OBJECT)
	       {
		  *json_out_str_tmp = '}';
		  json_out_str_tmp++;
	       }
	       else 
	       {
		  *json_out_str_tmp = ']';
		  json_out_str_tmp++;
	       }
	       
	       object_status_current = (object_status_t *)stack->search(stack->ret_count());
	       if(object_status_current && object_status_current->elements_done < object_status_current->elements)
	       {
		  *json_out_str_tmp = ',';
		  json_out_str_tmp++;
	       }
	    }
	 }
	 
	 json_object_old = json_object;
	 json_object = search(&key_pair);
      }while(json_object != JSON_END);
      
      return json_out_str;
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
	       {
		  add_element(JSON_ARRAY, key);
	       }
	       else 
	       {
		  add_element(JSON_OBJECT, key);		  
	       }
	       delete[] key;
	       key = NULL;
	       
	       start_pos += 2;
	       json_string_ptr++;
	    }
	    else 
	    {
	       bool ignore_space = false;
	       bool close_string = true;
	       if(*json_string_ptr == '"')
	       {
		  ignore_space = true;
		  close_string = false;
	       }
	       
	       while(1)
	       {
		  if(close_string)
		  {
		     if(is_delimiter(*json_string_ptr))
			break;
		  }
		  
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
		     close_string = true;
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
	    {
	       close_element();
	    }

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