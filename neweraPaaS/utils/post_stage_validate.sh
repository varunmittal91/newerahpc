#!/bin/sh
#
#  (C) 2013 Varun Mittal <varunmittal91@gmail.com>
#  NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v3
#
#  This file is part of NeweraHPC.
#
#  NeweraHPC is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation version 2 of the License.
#
#  NeweraHPC is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
#

function check_cmd_exists() {
   chk_command=$1 
   command -v $chk_command &> /dev/null
   if [ $? != 0 ]; then
      echo "Command: $chk_command Not found"
      return 1
   else
      return 0
   fi
}

# check service takes two arguments 1: for accepting service name and 2: for required(1 for default disabled and 0 for enabled)
function check_service() {
   service=$1 
   if [ $2 ]; then
      check_on=$2
   fi

   return_value=0
   chkconfig $service &> /dev/null
   if [ $? == 0 ]; then
      status="Activated on boot"
   else
      chkconfig --list $service &> /dev/null
      if [ $? != 0 ]; then
         status="Not found"
      else 
         status="Not activated on boot"
      fi
      return_value=1
   fi

   if [ $check_on ]; then
      if [ $check_on != $return_value ]; then
         echo -ne "Service: $service $status\n"
      fi
   fi

   return $return_value
}

function check_sshd_config() {
   if [ $1 ]; then
      path=$1
   else
      path="/etc/ssh/sshd_config"
   fi
   if [ ! -f $path ]; then
      echo "file not found $path"
      exit 1
   fi

   #### Checking for PasswordAuthentication, exit if enabled
   cat $path | grep ^PasswordAuthentication\ *\ yes > /dev/null 2>&1
   if [ $? == 0 ]; then
      echo "Error: PasswordAuthentication enabled in sshd_config"
   fi

   #### Checking for X11Forarding is disabled
   cat $path | grep ^X11Forwarding\ *\ yes > /dev/null 2>&1
   if [ $? == 0 ]; then
      echo "Error: X11Forwarding enabled in sshd_cofig"
   fi

   cat $path | grep ^RSAAuthentication\ *\ no > /dev/null 2>&1
   if [ $? == 0 ]; then
      echo "Error: RSAAuthentication disabled"
   fi

   cat $path | grep ^PubkeyAuthentication\ *\ no > /dev/null 2>&1
   if [ $? == 0 ]; then
      echo "Error: PubkeyAuthentication disabled"
   fi
}

###Check if chkconfig command exists, not present in debian based distributions
check_cmd_exists "chkconfig"

###Check if PasswordAuthentication,X11 formwarding and socks is disabled and keybased login is enabled
check_sshd_config

###Check if sshd is enabled during boot
check_service sshd 0
check_service squid 0
