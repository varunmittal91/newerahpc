/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#include <neweraHPC/grid.h>

using namespace neweraHPC;

namespace jarvis
{
#ifdef __cplusplus
   extern "C" 
   {
      nhpc_status_t jarvis_grid_plugin_register();
   
      nhpc_status_t plugin_init(plugin_details_t **plugin_details);
      nhpc_status_t plugin_exec(grid_instruction_t *instruction);
      nhpc_status_t plugin_processor(grid_instruction_t *instruction);
   }
#endif
};