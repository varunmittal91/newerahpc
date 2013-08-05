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

import errors

def getPAASRoot(cmd_arguments):
   try:
      paas_root = cmd_arguments['paas-root']
   except:
      errors.setError(errors.PAAS_EINVAL)
      return

   if not os.path.isdir(paas_root):
      errors.setError(errors.PAAS_ENOPAASROOT)
      return

   return paas_root

def getLXCRoot(cmd_arguments):
   try:
      lxc_root = cmd_arguments['lxc-root']
   except:
      errors.setError(errors.PAAS_EINVAL)
      return

   if not os.path.isdir(lxc_root):
      errors.setError(errors.PAAS_ENOPAASROOT)
      return

   return lxc_root

def getLocalNetPrefix(cmd_arguments):
   try:
      network_prefix = cmd_arguments['local-net-prefix']
   except:
      errors.setError(errors.PAAS_EINVAL)
      return

   return network_prefix

