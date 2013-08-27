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
import ConfigParser

import paas_errors

class Config:
   def __init__(self, config_file):
      config = ConfigParser.RawConfigParser()
      config.read(config_file)
      self.config = config
   def getValue(self, key):
      try:
         value = self.config.get('NeweraPaaS', key)
         return value
      except:
         return

def getPAASRoot(cmd_arguments):
   try:
      paas_root = cmd_arguments['paas-root']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   if not os.path.isdir(paas_root):
      paas_errors.setError(paas_errors.PAAS_ENOPAASROOT)
      return

   return paas_root

def getLXCRoot(cmd_arguments):
   try:
      lxc_root = cmd_arguments['lxc-root']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   if not os.path.isdir(lxc_root):
      paas_errors.setError(paas_errors.PAAS_ENOPAASROOT)
      return

   return lxc_root

def getLocalNetPrefix(cmd_arguments):
   try:
      network_prefix = cmd_arguments['local-net-prefix']
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   return network_prefix

def getLocalNetConfig(cmd_arguments):
   config_file = ''
   net_config  = {}
   try:
      config_file = cmd_arguments['config-file']
      config = ConfigParser.RawConfigParser()
      config.read(config_file)
      net_config['net-prefix'] = config.get('NeweraPaaS', 'local-net-prefix')
      net_config['net-type']   = config.get('NeweraPaaS', 'local-net-type')
      net_config['net-link']   = config.get('NeweraPaaS', 'local-net-link')
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return

   return net_config

def getMysqlConfig(cmd_arguments):
   config_file  = ''
   mysql_config = {}
   try:
      config_file = cmd_arguments['config-file']
      config = ConfigParser.RawConfigParser()
      config.read(config_file)
      mysql_config['sql_host']   = config.get('NeweraPaaS', 'mysql-host')
      mysql_config['sql_user']   = config.get('NeweraPaaS', 'mysql-user')
      mysql_config['sql_passwd'] = config.get('NeweraPaaS', 'mysql-passwd')
      mysql_config['sql_db']     = config.get('NeweraPaaS', 'mysql-db')
   except:
      paas_errors.setError(paas_errors.PAAS_EINVAL)
      return
   return mysql_config


      
