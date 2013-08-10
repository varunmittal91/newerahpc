#!/usr/bin/python

#
#       (C) 2013 Varun Mittal <varunmittal91@gmail.com>
#       NeweraHPC program is distributed under the terms of the GNU General Public License v3
#
#       This file is part of NeweraHPC.
#
#       NeweraHPC is free software: you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation version 3 of the License.
#
#       NeweraHPC is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public License
#       along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
#

import os

def start(container_name):
   cmd = "lxc-start -d -n " + container_name
   status = os.system(cmd)
   if status != 0:
      return -1
   return status

def stop(container_name):
   cmd = "lxc-stop -n " + container_name
   print cmd
   os.system(cmd) 
   if os.system(cmd) != 0:
      return -1
   else:
      return 0

def shutdown(container_name):
   cmd = "lxc-shutdown -n " + container_name
   if os.system(cmd) != 0:
      return -1
   else:
      return 0
