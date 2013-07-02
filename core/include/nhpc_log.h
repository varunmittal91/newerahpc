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

#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>

static bool log_on[3];

enum Level { 
   Debug, 
   Warning, 
   Info, 
   Error 
};
   
static std::ostream& GetStream() { return std::cout; }
static bool          IsLevelActive(Level l) { return true; }

#define LOG_ERROR(M)   do { if (IsLevelActive(Error))                      (GetStream() << "ERR: " << M << "\n"); } while (false)
#define LOG_INFO(M)    do { if (IsLevelActive(Info)    && log_on[Info])    (GetStream() << "INF: " << M << "\n"); } while (false)
#define LOG_WARNING(M) do { if (IsLevelActive(Warning) && log_on[Warning]) (GetStream() << "WRN: " << M << "\n"); } while (false)
#define LOG_DEBUG(M)   do { if (IsLevelActive(Debug)   && log_on[Debug])   (GetStream() << "DBG: " << M << "\n"); } while (false)
   
#define log_is_debug_enabled()    (log_on[Debug])
#define log_is_info_enabled()     (log_on[Info])
#define log_is_warning_enabled()  (log_on[Warning])

#endif