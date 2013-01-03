/*
 *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _JSON_H_
#define _JSON_H_

#include "rbtree.h"
#include "constants.h"
#include "strings.h"

namespace neweraHPC
{
   extern const char* JSON_OBJECT_STRINGS[5];
   
   enum json_object
   {
      JSON_INCOMPLETE,
      JSON_END,
      JSON_ARRAY,
      JSON_OBJECT,
      JSON_OBJECT_NOT_FOUND,
      JSON_STRING,
      JSON_NUMBER,
      JSON_TRUE,
      JSON_FALSE,
      JSON_NULL,
      JSON_OBJECT_CLOSE, // Not to be used, for internal structure only
      JSON_ARRAY_CLOSE   // Not to be used, for internal structure only
   };
   
   enum json_mode
   {
      JSON_INSERT,
      JSON_UPDATE
   };
   
   class json_object_t
   {
   private:
      key_pair_t *root_key_pair;
      rbtree *search_queue;
      
   public:
      json_object_t(key_pair_t *_root_key_pair);
      
      const char *operator[](const char *key);
      const char *operator[](int key);
   };
   
   class json_t
   {
   private:
      struct key_pair_t
      {
	 char     *key;
	 int	  json_object;
	 
	 union value_pair_t
	 {
	    struct branches_t
	    {
	       rbtree *branch;
	       rbtree *branch_object;
	    };
	    
	    char       *value;
	    branches_t  branches;
	 };
	 
	 bool         value_string;
	 value_pair_t value_pair;
      };
      struct search_elem_t
      {
	 rbtree *branch;
	 int       position;
      };
      
      rbtree   *root;
      key_pair_t *root_key_pair;
      key_pair_t *current_key_pair;
      
      int   final_json_length;
      char *final_json_string;

      int traverse(key_pair_t **_key_pair);
      rbtree *search_queue;
      
      void initialize();
      void reinitialize();
      
      /* json object search utility internal mechanism */
      key_pair_t *search(int json_object, int *key_num, const char *key_str, const char **response);
      
      /* json object update utility internal mechanism */
      nhpc_status_t update_value(key_pair_t *key_pair, const char *input);
   public:
      json_t();
      ~json_t();
      
      nhpc_status_t add_element(int json_object, const char *input1 = NULL, const char *input2 = NULL);
      nhpc_status_t close_element();
      
      /* json object search utility */
      const char *operator[](const char *key);
      const char *operator[](int key);
      int count();
      void reinitialize_search();
      
      /* json object update utility */
      nhpc_status_t update(int key,         const char *input = NULL);
      nhpc_status_t update(const char *key, const char *input = NULL);
      
      nhpc_status_t build_structure(const char *json_string);
      const char *get_string();
      
      bool is_delimiter(char in_char);
      void print();
   };
   
   
   /*
   class nhpc_json_t
   {
   private:
      struct search_elem_t
      {
	 rbtree *branch;
	 int position;
      };  
      struct stream_elem_t
      {
	 int json_object;
      };
      
      int stream_length;
      int current_level;
      rbtree *nodes;
      rbtree *backtrack;
      rbtree *search_queue;
   public:
      struct key_pair_t
      {
	 char *key;
	 void *value;
	 int json_object;
	 int level;
      };

      nhpc_json_t();
      ~nhpc_json_t();
      nhpc_status_t add_element(int json_object, const char *key = NULL, const void *value = NULL);
      nhpc_status_t close_element();
      int search(key_pair_t **key_pair, int *child_count = NULL);

      nhpc_status_t search(const char *key);
      
      const char *get_stream();
      nhpc_status_t build_structure(const char *json_string);
      bool is_delimiter(char in_char);
      void print();
   };
    */
};

#endif