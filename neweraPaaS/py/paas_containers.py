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
import shutil

import paas_errors
import paas_config
import paas_images
import paas_network
import paas_mount
import paas_lxc_interface

def createContainer(cmd_arguments):
   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      paas_errors.paasPerror("getPAASRoot(), createContainer()")
      return

   image_details = paas_images.checkImage(cmd_arguments, paas_root)
   if not image_details:
      paas_errors.paasPerror("checkImage(), createContainer")
      return
	
   net_config = paas_config.getLocalNetConfig(cmd_arguments)
   if not net_config:
      paas_errors.paasPerror("getLocalNetConfig() failed, createContainer()")
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
            paas_errors.setError(paas_errors.PAAS_EACCES)
            paas_errors.paasPerror("createContainer()")
            return
   instance_path = _instance_path

   hwaddr1 = paas_network.generateHWADDR()

   local_ipaddr_prefix = paas_config.getLocalNetPrefix(cmd_arguments)
   if not local_ipaddr_prefix:
      paas_errors.paasPerror("getLocalNetPrefix(), createContainer()")
      return

   local_ipaddr = local_ipaddr_prefix + "." + str(i)

   container_config = instance_path + "/neweraPaaS.config"

   config = ConfigParser.RawConfigParser()
   config.add_section("InstanceDetails")
   config.set("InstanceDetails", "image-name", image_details[0])
   config.set("InstanceDetails", "container-name", "instance-" + str(i))
   config.set("InstanceDetails", "hwaddr1", hwaddr1)
   config.set("InstanceDetails", "local-ipaddr", local_ipaddr)
   with open(container_config, 'wb') as configfile:
      config.write(configfile)

   lxc_root = paas_config.getLXCRoot(cmd_arguments)
   rootfs   = lxc_root + "/instance-" + str(i) + "/rootfs"
   rootfs_fstab = lxc_root + "/instance-" + str(i) + "/fstab"

   shutil.copy(image_details[1] + "/config", instance_path + "/config")
   file = open(instance_path + "/config", 'a')
   file.write("lxc.rootfs = " + rootfs + "\n")
   file.write("lxc.mount = " + rootfs_fstab + "\n")

   file.write("lxc.network.type = " + net_config['net-type'] + "\n")
   file.write("lxc.network.link = " + net_config['net-link'] + "\n")
   file.write("lxc.network.ipv4 = " + local_ipaddr + "\n")
   file.write("lxc.network.flags = up" + "\n")

   file.write("lxc.network.hwaddr = " + hwaddr1 + "\n")
   file.write("lxc.utsname = instance-" + str(i) + "\n")

   file.close()

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
   if paas_mount.mountContainer(lxc_path, image_path, instance_path) == -1:
      paas_errors.paasPerror("mountContainer() failed, _startContainer()")
      os.rmdir(lxc_path)
      return -1

   if paas_lxc_interface.start(container_name) == -1:
      paas_errors.paasPerror("lxc_interface_start(), _startContainer()")
      return -1

   return 0

def startContainer(cmd_arguments):
   container_name = ''
   try:
      container_name = cmd_arguments['container-name']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      paas_errors.paasPerror("startContainer()")

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      paas_errors.paasPerror("getPAASRoot(), startContainer()")
      return 
   lxc_root = paas_config.getLXCRoot(cmd_arguments)
   if not lxc_root:
      paas_errors.paasPerror("getLXCRoot(), startContainer()")
      return

   container_config = paas_root + "/instances/" + container_name
   if not os.path.isdir(container_config):
      paas_errors.setError(paas_errors.PAAS_ENNOENT)
      paas_errors.paasPerror("startContainer()")
      return
   container_config += "/neweraPaaS.config"

   lxc_path = lxc_root + "/" + container_name
   if os.path.isdir(lxc_path):
      paas_errors.setError(paas_errors.PAAS_ENACTIVE)
      paas_errors.paasPerror("startContainer()")
      return
   _startContainer(container_config, lxc_root, paas_root)

def _stopContainer(container_name, lxc_path):
   if paas_lxc_interface.shutdown(container_name) == -1:
      return -1

   if paas_mount.umountContainer(lxc_path) == -1:
      paas_errors.paasPerror("_umountContainer() failed, _stopContainer()")
      return -1
   try:
      os.rmdir(lxc_path)
   except:
      return -1

def stopContainer(cmd_arguments):
   container_name = ''
   try:
      container_name = cmd_arguments['container-name']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      paas_errors.paasPerror("stopContainer()")
      return

   paas_root = paas_config.getPAASRoot(cmd_arguments)
   if not paas_root:
      paas_errors.paasPerror("getPAASRoot(), startContainer()")
      return
   lxc_root = paas_config.getLXCRoot(cmd_arguments)
   if not lxc_root:
      paas_errors.paasPerror("getLXCRoot(), startContainer()")
      return
      lxc_path = lxc_root + "/" + container_name

   container_config = paas_root + "/instances/" + container_name + "/neweraPaaS.config"
   if not os.path.isfile(container_config):
      paas_errors.setError(paas_errors.PAAS_ENNOENT)
      paas_errors.paasPerror("stopContainer()")
      return

   lxc_path = lxc_root + "/" + container_name
   if not os.path.isdir(lxc_path):
      paas_errors.setError(paas_errors.PAAS_ENNOACTIVE)
      paas_errors.paasPerror("startContainer()")
      return
   
   if _stopContainer(container_name, lxc_path) == -1:
      paas_errors.paasPerror("_stopContainer() failed, stopContainer")
      return -1
   return 0

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
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   return paas_root

def shutdownContainer(cmd_arguments):
   container_name = ''
   try:
      container_name = cmd_arguments['container-name']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      paas_errors.paasPerror("stopContainer()")
      return   

   paas_lxc_interface.shutdown()
   return   
