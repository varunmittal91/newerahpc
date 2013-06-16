/*
 *      (C) 2013 Varun Mittal <varunmittal91@gmail.com>
 *      NeweraPaaS program is distributed under the terms of the GNU General Public License v3
 *
 *      This file is part of NeweraPaaS.
 *
 *      NeweraHPC is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation version 2 of the License.
 *
 *      NeweraPaaS is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CREATE_CONTAINER_H_
#define _CREATE_CONTAINER_H_

#include <neweraHPC/constants.h>

#include "lxc_interface.h"
#include "lxc_data.h"

namespace neweraHPC
{
	nhpc_status_t create_container(lxc_container_t *container);
};

#endif
