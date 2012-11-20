/*
 *	(C) 2012 Varun Mittal <varunmittal91@gmail.com>
 *	jarvis program is distributed under the terms of the GNU General Public License v3
 *
 *	This file is part of jarvis.
 *
 *	jarvis is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation version 3 of the License.
 *
 *	jarvis is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with jarvis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <neweraHPC/neweraHPC.h>

#include <include/jarvis.h>
#include <include/words.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   thread_manager_t *thread_manager;
   
   void jarvis_init(int argc, char **argv)
   {
      nhpc_status_t nrv;
      
      neweraHPC_init(argc, argv);

      nrv = init_word_net_database();
      if(nrv != NHPC_SUCCESS)
	 return;
      
      thread_manager = new thread_manager_t;
      jarvis_data.init();
      jarvis_data.init_morphological_database();
   }
};
