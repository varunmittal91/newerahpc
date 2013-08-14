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
import sys
import errno
import getopt
import ConfigParser

import neweraPaaS

def updateFunctions(function_list):
   function_list['create']   = neweraPaaS.containers.createContainer
   function_list['destroy']  = neweraPaaS.containers.destroyContainer
   function_list['start']    = neweraPaaS.containers.startContainer
   function_list['stop']     = neweraPaaS.containers.stopContainer
   function_list['shutdown'] = neweraPaaS.containers.shutdownContainer

def main(argv):

   cmd_arguments = {}

   paas_root        = ''
   lxc_root         = ''
   config_file      = ''
   paas_user        = ''
   local_net_prefix = ''

# evaluating command line arguments
#    -i image name
#    -n instance name
#    -c config
#    -k command
#    -p pass root
#    -l lxc root
   try:
      opts, args = getopt.getopt(argv, "i:n:c:k:l:p:")
   except getopt.GetoptError:
      print "neweraPaaS -c <command>"
      sys.exit(1)
   for opt, arg in opts:
      if opt in ('-i'):
         cmd_arguments['image-name'] = arg
      elif opt in ('-n'):
         cmd_arguments['container-name'] = arg
      elif opt in ('-c'):
         cmd_arguments['config-file'] = arg
         config = ConfigParser.RawConfigParser()
         try:
            config.read(arg)
            lxc_root         = config.get('NeweraPaaS', 'lxc-root')
            paas_root        = config.get('NeweraPaaS', 'paas-root')
            local_net_prefix = config.get('NeweraPaaS', 'local-net-prefix')
            paas_user        = config.get('NeweraPaaS', 'paas-user')
         except:
            print "config file could not be read"
            sys.exit(2)
      elif opt in ('-k'):
         cmd_arguments['cmd'] = arg
      elif opt in ('-l'):
         lxc_root = arg
      elif opt in ('-p'):
         paas_root = arg

   try:
      cmd = cmd_arguments['cmd']
   except:
      print "No command given"
      sys.exit(2)   

   if paas_root:
      cmd_arguments['paas-root'] = paas_root
   if lxc_root:
      cmd_arguments['lxc-root'] = lxc_root
   if local_net_prefix:
      cmd_arguments['local-net-prefix'] = local_net_prefix

   if paas_user:
      from pwd import getpwnam  
      try: 
         uid = getpwnam(paas_user).pw_uid
         os.setuid(uid)
      except os.error as error:
         if error.errno == neweraPaaS.errors.PAAS_ENOPT:
       	    neweraPaaS.errors.setError(neweraPaaS.errors.PAAS_ENOPT)
         else:
            neweraPaaS.errors.setError(neweraPaaS.errors.PAAS_EINVALUSER)
         return -1

   function_list = {}
   updateFunctions(function_list)

   try:
      func = function_list[cmd]
   except:
      sys.exit(2)

   return func(cmd_arguments)

if __name__ == "__main__":
   if main(sys.argv[1:]) == -1:
      neweraPaaS.errors.paasPerror("main() failed")
