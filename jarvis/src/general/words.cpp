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

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <string>
#include <neweraHPC/neweraHPC.h>
#include <neweraHPC/list.h>

#include <include/general.h>
#include <include/words.h>
#include <include/jarvis_data.h>

using namespace std;
using namespace neweraHPC;

namespace jarvis
{
   _pos_codes pos_codes[POS_COUNT] = {1, 'r', "adv", 2, 'a', "adj", 4, 'n', "noun", 8, 'v', "verb", 16, 's', NULL};
};