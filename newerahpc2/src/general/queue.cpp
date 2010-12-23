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

#include <network.h>

namespace newera_network{
	queue::queue(){
        count = 0;
        first = NULL;
		current = NULL;
	}
	void queue::queue_manual(){
		count = 0;
		first = NULL;
		current = NULL;
	}
	queue::~queue(){
		empty();
	}
	void queue::operator+=(void *in_p){
        queue_elem *new_elem = (queue_elem *)malloc(sizeof(queue_elem));
        new_elem->next = NULL;
        new_elem->data = in_p;
        count++;                                                     
        if(first==NULL){
			first=new_elem;
			current = new_elem;
		}
        else{
			current->next = new_elem;
			current = new_elem;
        }
	}
	void *queue::operator[](int id){
        if(id>=count)return NULL;
        queue_elem *tmp_elem = first;
        for(int cntr=0;cntr<id;cntr++){
			tmp_elem = tmp_elem->next;
        }
        return tmp_elem->data;
	}
	void queue::operator-=(int id){
        if(id>=count)return;
        queue_elem *tmp_elem = first;
        for(int cntr=0;cntr<(id-1);cntr++){
			tmp_elem = tmp_elem->next;
        }
        int *data = (int *)tmp_elem->data;
        if(id==0){
			if(count==1){
				first=NULL;
				current=NULL;
				free(first);
			}
			else{
				first=first->next;
				free(tmp_elem);
			}
        }
        else{
			if(id==(count-1)){
				current = tmp_elem;
			}
			queue_elem *tmp_elem_1 = tmp_elem->next;
			tmp_elem->next = tmp_elem_1->next;
			free(tmp_elem_1);
        }
        count--;
	}
	void queue::operator-=(void *in_p){
        if(count==0)return;
        queue_elem *tmp_elem = first;
        for(int cntr=0;cntr<count;cntr++){
			if(tmp_elem->data==in_p){
				operator-=(cntr);
				return;
			}
			tmp_elem = tmp_elem->next;
        }
	}
	void queue::empty(){
		int cntr=0;
		while(cntr<count){	
			operator-=(cntr);
			cntr++;
		}
	}
};