#!/usr/bin/python

#
#       (C) 2013 Varun Mittal <varunmittal91@gmail.com> 
#       NeweraHPC program is distributed under the terms of the GNU General Public License v3
#
#       This file is part of NeweraHPC.
#
#	NeweraHPC is free software: you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation version 3 of the License.
#
#       NeweraHPC is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#       along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
#
   
import os
import paas_errors

def mountContainer(lxc_path, image_path, instance_path):
   print "Mounting container file system:"
   print "\t Target:" + lxc_path
   print "\t Image:" + image_path
   print "\t Instance:" + instance_path

   branch = "lowerdir=" + image_path + ",upperdir=" + instance_path
   cmd = "mount -t overlayfs overlayfs -o" + branch + " " + lxc_path
   status = os.system(cmd)
   if status != 0:
      paas_errors.setError(paas_errors.PAAS_EMOUNT)
      return -1

   return status

def umountContainer(lxc_path):
   print "Umounting container file system:"
   print "\t Target:" + lxc_path

   cmd = "umount " + lxc_path
   status = os.system(cmd)
   if status != 0:
      paas_errors.setError(paas_errors.PAAS_EUMOUNT)
      return

   return status

