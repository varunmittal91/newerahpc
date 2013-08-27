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

import sys
import errno
import traceback

paas_errno     = 0
paas_traceback = 0

global PAAS_ENOPT
global PAAS_ENOENT      
global PAAS_EACCES
global PAAS_EINVAL      
global PAAS_ENOLXCROOT  
global PAAS_ENOPAASROOT 
global PAAS_ENNOENT
global PAAS_EINOENT
global PAAS_ENEXISTS    
global PAAS_EIEXISTS
global PAAS_EMOUNT
global PAAS_EUMOUNT
global PAAS_ESTARTLXC
global PAAS_ESTOPLXC
global PAAS_EINVALUSER
global PAAS_ENOUMOUNT
global PAAS_ENOPLATFRM
global PAAS_ENOMYSQL
global PAAS_ENOHOST

PAAS_ENOPT       = 1            # Operation not permitted
PAAS_ENOENT      = errno.ENOENT # No such file or directory
PAAS_EACCES      = errno.EACCES # Permission denied
PAAS_EINVAL      = errno.EINVAL # Invalid arguments
PAAS_ENOLXCROOT  = 104          # No LXC root
PAAS_ENOPAASROOT = 105          # No PAAS root
PAAS_ENEXISTS    = 106          # Container exists
PAAS_EIEXISTS    = 107          # Image exists
PAAS_ENNOENT     = 108          # No such container
PAAS_EINOENT     = 109          # No such image
PAAS_ENACTIVE    = 110          # Container already running
PAAS_ENNOACTIVE  = 111          # Container not running
PAAS_EMOUNT      = 112          # Unable to mount image
PAAS_EUMOUNT     = 113          # Unable to unmount image
PAAS_ESTARTLXC   = 114          # Unable to start container
PAAS_ESTOPLXC    = 115          # Unable to stop container
PAAS_EINVALUSER  = 116          # Invalid paas user given
PAAS_ENOUMOUNT   = 117          # No union mount support available
PAAS_ENOPLATFRM  = 118          # Platform not supported
PAAS_ENOMYSQL    = 119          # Error connecting mysql
PAAS_ENOHOST     = 120          # Error host not found or down


PAAS_ERROR_STRINGS = {}
PAAS_ERROR_STRINGS[PAAS_ENOPT]       = "Operation not permitted"
PAAS_ERROR_STRINGS[PAAS_ENOENT]      = "No such file or directory" 
PAAS_ERROR_STRINGS[PAAS_EACCES]      = "Permission denied"
PAAS_ERROR_STRINGS[PAAS_EINVAL]      = "Invalid arguments"
PAAS_ERROR_STRINGS[PAAS_ENOLXCROOT]  = "No LXC root"
PAAS_ERROR_STRINGS[PAAS_ENOPAASROOT] = "No PAAS root"
PAAS_ERROR_STRINGS[PAAS_ENNOENT]     = "No such container"
PAAS_ERROR_STRINGS[PAAS_EINOENT]     = "No such image"
PAAS_ERROR_STRINGS[PAAS_ENEXISTS]    = "Container exists"
PAAS_ERROR_STRINGS[PAAS_EIEXISTS]    = "Image exists"
PAAS_ERROR_STRINGS[PAAS_ENACTIVE]    = "Container already running"
PAAS_ERROR_STRINGS[PAAS_ENNOACTIVE]  = "Container not running"
PAAS_ERROR_STRINGS[PAAS_EMOUNT]      = "Unable to mount image"
PAAS_ERROR_STRINGS[PAAS_EUMOUNT]     = "Unable to unmount image"
PAAS_ERROR_STRINGS[PAAS_ESTARTLXC]   = "Unable to start container"
PAAS_ERROR_STRINGS[PAAS_ESTOPLXC]    = "Unable to stop container"
PAAS_ERROR_STRINGS[PAAS_EINVALUSER]  = "Invalid paas user give"
PAAS_ERROR_STRINGS[PAAS_ENOUMOUNT]   = "No union mount support available"
PAAS_ERROR_STRINGS[PAAS_ENOPLATFRM]  = "Platform not supported"
PAAS_ERROR_STRINGS[PAAS_ENOMYSQL]    = "Error connecting mysql"
PAAS_ERROR_STRINGS[PAAS_ENOHOST]     = "Error host not found or down"

def setError(error, identifier = ''):
   global paas_errno
   global paas_errno_identifier
   global paas_traceback
   paas_errno            = error
   paas_traceback        = traceback.extract_stack()
   paas_errno_identifier = identifier

def paasPerror(string):
   error_str  = ''
   perror_str = ''

   if string:
      if paas_errno_identifier:
         error_str = string + ",identifier: " + paas_errno_identifier + " : "
      else:
         error_str = string + ": "
   try:
      perror_str = PAAS_ERROR_STRINGS[paas_errno] 
   except:
      perror_str = PAAS_ERROR_STRINGS[PAAS_ENOENT]
   
   error_str += perror_str
   print error_str
   print paas_traceback
