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

import errno

paas_errno = 0

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

PAAS_ERROR_STRINGS = {}
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

def setError(error):
   global paas_errno
   paas_errno = error

def paasPerror(string):
   error_str  = ''
   perror_str = ''

   if string:
      error_str = string + ": "
   try:
      perror_str = PAAS_ERROR_STRINGS[paas_errno] 
   except:
      perror_str = PAAS_ERROR_STRINGS[PAAS_ENOENT]
   
   error_str += perror_str
   print error_str
