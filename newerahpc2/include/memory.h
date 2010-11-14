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

namespace newera_network{
	class mem{
		struct mem_element{
			void *data;
			size_t size;
		};
		queue *elements;
	public:
		mem();
		~mem();
		void add_mem(void *,size_t);
		void rem_mem(void *);
	};
	void *alloc(size_t);
	void dalloc(void *);
	extern mem *mem_obj;
};