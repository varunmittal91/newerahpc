/*
 *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
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

#ifndef _GRID_PLUGIN_RANGE_H_
#define _GRID_PLUGIN_RANGE_H_

#include <neweraHPC/constants.h>

#include "grid_plugin.h"
#include "grid_data.h"
#include "grid_instruction.h"

namespace neweraHPC
{
   nhpc_status_t grid_plugin_range_init(plugin_details_t **plugin_details);
   nhpc_status_t grid_plugin_range_exec(grid_instruction_t *instruction);
   nhpc_status_t grid_plugin_range_processor(grid_instruction_t *instruction);
};

#endif