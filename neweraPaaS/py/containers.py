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
import errno
import ConfigParser

import errors
import paas_images
import paas_config
import paas_network
import paas_mount

def createContainer(cmd_arguments):

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      errors.paasPerror("getPAASRoot(), createContainer()")
      return

   image_details = paas_images.checkImage(cmd_arguments, paas_root)
   if not image_details:
      errors.paasPerror("checkImage(), createContainer")
      return
	
   instance_path = paas_root + "/instances/" 
   i = 1
   while 1:
      i += 1
      _instance_path = instance_path + "instance-" + str(i)
      
      try:
         os.mkdir(_instance_path)
         break
      except OSError as exception:
         if exception.errno != errno.EEXIST:
            errors.setError(errors.PAAS_EACCES)
            errors.paasPerror("createContainer()")
            raise
            return
   instance_path = _instance_path

   hwaddr1 = paas_network.generateHWADDR()
   hwaddr2 = paas_network.generateHWADDR()

   local_ipaddr_prefix = paas_config.getLocalNetPrefix(cmd_arguments)
   if not local_ipaddr_prefix:
      errors.paasPerror("getLocalNetPrefix(), createContainer()")
      return

   local_ipaddr = local_ipaddr_prefix + "." + str(i)

   container_config = instance_path + "/neweraPaaS.config"

   config = ConfigParser.RawConfigParser()
   config.add_section("InstanceDetails")
   config.set("InstanceDetails", "image-name", image_details[0])
   config.set("InstanceDetails", "container-name", "instance-" + str(i))
   config.set("InstanceDetails", "hwaddr1", hwaddr1)
   config.set("InstanceDetails", "hwaddr2", hwaddr2)
   config.set("InstanceDetails", "local-ipaddr", local_ipaddr)
   with open(container_config, 'wb') as configfile:
      config.write(configfile)

############### LXC Container, starting container
   _startContainer(container_config, paas_config.getLXCRoot(cmd_arguments),
                   paas_root)
############### 

   return i

def destroyContainer(cmdline_arguments):
   print "Destroying containers"

def _startContainer(container_config, lxc_root, paas_root):
   print "Starting container:" + container_config
   config = ConfigParser.RawConfigParser()
   config.read(container_config)
   container_name = config.get("InstanceDetails", "container-name")
   image_name     = config.get("InstanceDetails", "image-name")

   lxc_path      = lxc_root + "/" + container_name 
   instance_path = paas_root + "/instances/" + container_name
   image_path    = paas_root + "/images/" + image_name
   os.mkdir(lxc_path)
   paas_mount.mountContainer(lxc_path, image_path, instance_path)
   return 0

def startContainer(cmd_arguments):
   container_name = ''
   try:
      container_name = cmd_arguments['container-name']
   except:
      errors.setError(errors.PAAS_EINVAL)
      errors.paasPerror("startContainer()")

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      errors.paasPerror("getPAASRoot(), startContainer()")
      return 
   lxc_root = paas_config.getLXCRoot(cmd_arguments)
   if not lxc_root:
      errors.paasPerror("getLXCRoot(), startContainer()")
      return

   container_config = paas_root + "/instances/" + container_name
   if not os.path.isdir(container_config):
      errors.setError(errors.PAAS_ENNOENT)
      errors.paasPerror("startContainer()")
      return
   container_config += "/neweraPaaS.config"

   lxc_path = lxc_root + "/" + container_name
   if os.path.isdir(lxc_path):
      errors.setError(errors.PAAS_ENACTIVE)
      errors.paasPerror("startContainer()")
      return
   _startContainer(container_config, lxc_root, paas_root)

def _stopContainer(container_config, lxc_root):
   return

def stopContainer(cmd_arguments):
   container_name = ''
   try:
      container_name = cmd_arguments['container-name']
   except:
      errors.setError(errors.PAAS_EINVAL)
      errors.paasPerror("startContainer()")

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      errors.paasPerror("getPAASRoot(), startContainer()")
      return
   lxc_root = paas_config.getLXCRoot(cmd_arguments)
   if not lxc_root:
      errors.paasPerror("getLXCRoot(), startContainer()")
      return
      lxc_path = lxc_root + "/" + container_name

   container_config = paas_root + "/instances/" + container_name + "/neweraPaaS.config"
   if not os.path.isfile(container_config):
      errors.setError(errors.PAAS_ENNOENT)
      errors.paasPerror("stopContainer()")
      return

   lxc_path = lxc_root + "/" + container_name
   if not os.path.isdir(lxc_path):
      errors.setError(errors.PAAS_ENNOACTIVE)
      errors.paasPerror("startContainer()")
      return
   
   paas_mount.umountContainer(lxc_path)
   os.rmdir(lxc_path)

def checkContainer(cmd_arguments):

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      print "no pass root" 
      return 

   container_name = ''

   try:
      conatiner_name = cmd_arguments['container_name']
   except:
      print "no conrainer name"
      errors.setError(errors.PAAS_EINVAL)
      return

   return paas_root

   
