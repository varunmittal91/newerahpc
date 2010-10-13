//Copyright 2009 Varun Mittal ©
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

#include "data.h"
#include "basic_math.h"
#include "io.h"
#include "compare.h"
#include "basic.h"
#include "adv_math.h"
#include "old_math.h"
#include "string_mpi.h"

namespace newera_mpi{
  extern newera_hpc_data* init_data();
  extern void delete_data(newera_hpc_data*);
  extern void re_init_data(newera_hpc_data*);
  extern void zero(newera_hpc_data*);
  extern void zero_manual(newera_hpc_data*);
};
