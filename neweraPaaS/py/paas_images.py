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
import paas_config

def checkImageName(image_name, paas_root):
   image_path = paas_root + "/images/" + image_name
   if not os.path.isdir(image_path):
      paas_errors.setError(paas_errors.PAAS_EINOENT)
      return
   return image_path

def checkImage(cmd_arguments, paas_root):
   image_name = ''
   image_path = ''
   
   try:
      image_name = cmd_arguments['image-name']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   image_path = checkImageName(image_name, paas_root)
   if not image_path:
      return

   return (image_name, image_path)
