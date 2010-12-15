//Copyright 2010 Varun Mittal ©
//newerahpc program is distributed under the terms of the GNU General Public License
//
//This file is part of newerahpc.
//
//newerahpc is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation version 2 of the License.
//
//newerahpc is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with newerahpc.  If not, see <http://www.gnu.org/licenses/>.

#define NOT_REGISTER 0
#define REGISTER 1

namespace newera_network{
	class mem{
		struct mem_element{
			void *data;
			size_t size;
		};
		queue *elements;
		pthread_mutex_t *mutex;
	public:
		mem();
		~mem();
		void lock();
		void unlock();
		void add_mem(void *,size_t);
		void rem_mem(void *);
		void *expand_mem(void *,size_t);
		void rem_mem_clean(void *);
		void *locate(void *);
	};
	void *alloc(size_t);
	void *expand_mem(void *,size_t);
	extern mem *mem_obj;
	extern int mem_obj_status;
	
	/*
	template <class T,class U>
	T* alloc(size_t in_size,U param1){
		T *data = new T [in_size](param1);
		(*mem_obj).add_mem((void *)data,sizeof(T)*in_size);
		return data;		
	}
	template <class T>
	T *alloc(size_t in_size){
		T *data = new T [in_size];
		(*mem_obj).add_mem((void *)data,sizeof(T)*in_size);
		return data;
	}
	template <class T>
	T *alloc(){
		T *data = new T;
		(*mem_obj).add_mem(data,sizeof(T));
		return data;
	}
	template <class T,class U>
	T* alloc(U param1){
		T *data = new T (param1);
		(*mem_obj).add_mem((void *)data,sizeof(T));
		return data;		
	}
	template <class T>
	void dalloc(T *in_p){
		mem_obj->rem_mem(in_p);
	}
	 */
};
