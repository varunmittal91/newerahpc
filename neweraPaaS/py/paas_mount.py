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

import paas_errors

if os.system("modprobe aufs") == 0:
   import paas_mount_aufs as _mount
elif os.system("modprobe overlayfs") == 0:
   import paas_mount_overlayfs as _mount
else:
   print "no union mount support"
   exit(2)

def mountContainer(lxc_path, image_path, instance_path):
   _mount.mountContainer(lxc_path, image_path, instance_path)

def umountContainer(lxc_path):
   _mount.umountContainer(lxc_path)   
