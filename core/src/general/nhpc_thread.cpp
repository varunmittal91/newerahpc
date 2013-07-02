/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *	NeweraHPC program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of NeweraHPC.
 *
 *	NeweraHPC is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	NeweraHPC is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <include/neweraHPC.h>

pthread_attr_t thread_attr;
nhpc_uint_t    nthreads;
nhpc_uint_t    max_threads;

nhpc_status_t nhpc_init_threads(int n, nhpc_size_t size) {
   int rv = pthread_attr_init(&thread_attr);
   
   if(rv < 0)
      return errno;
   if((rv = pthread_attr_setstacksize(&thread_attr, size)) < 0)
      return errno;
   if((rv =  pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED)) < 0) {
      LOG_ERROR("Unable to set thread attribute: PTHREAD_CREATE_DETACHED");
      return errno;
   }
   
   max_threads = n;
   nthreads    = 0;
   
   return NHPC_SUCCESS;
}

nhpc_status_t nhpc_create_thread(pthread_t *tid, nhpc_thread_value_t func, void *arg) {
   int rv;
   
   if(nthreads >= max_threads) {
      LOG_ERROR("thread_create() failed, reached maximum limit");
      return NHPC_FAIL;
   }
   
   if((rv = pthread_create(tid, &thread_attr, func, arg)) < 0)
      return errno;
   
   nthreads++;
   
   return NHPC_SUCCESS;
}

void nhpc_exit_thread() {
   nthreads--;
   
   //pthread_exit(NULL);
}


nhpc_status_t nhpc_mutex_init(nhpc_mutex_t *mutex) {
   int rv;
   
   rv = pthread_rwlock_init(&(mutex->lock) , NULL);
   if(rv < -1)
   {
      perror("error");
      return errno;
   }
   
   return NHPC_SUCCESS;
}

nhpc_status_t nhpc_mutex_lock(nhpc_mutex_t *mutex, int for_read) {
   int rv;
   
   if(for_read)
      rv = pthread_rwlock_rdlock(&(mutex->lock));
   else 
      rv = pthread_rwlock_wrlock(&(mutex->lock));
   
   if(rv < 0) {
      perror("lock failed");
   }
   
   return NHPC_SUCCESS;
}

nhpc_status_t nhpc_mutex_trylock(nhpc_mutex_t *mutex, int for_read) {
   nhpc_status_t rv;
   
   if(for_read)
      rv = pthread_rwlock_tryrdlock(&(mutex->lock));
   else 
      rv = pthread_rwlock_trywrlock(&(mutex->lock));
   
   if(rv == 0)
      return NHPC_SUCCESS;
   return rv;
}

nhpc_status_t nhpc_mutex_unlock(nhpc_mutex_t *mutex, int for_read) {
   pthread_rwlock_unlock(&(mutex->lock));
}



