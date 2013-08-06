#
#   (C) 2011 Varun Mittal <varunmittal91@gmail.com>
#   NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v2
#
#   This file is part of NeweraHPC_webstorage.
#
#   NeweraHPC_webstorage is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation version 2 of the License.
#
#   NeweraHPC_webstorage is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with NeweraHPC_webstorage.  If not, see <http://www.gnu.org/licenses/>.
#

import ldap
import ldap.modlist as modlist
import os

try:
   os.system("echo run script > /tmp/run")
   ldap_conn = ldap.initialize("ldap://localhost:389")
   ldap_conn.simple_bind_s("cn=admin, dc=newerahpc, dc=com", "password")

   dn_people = "ou=people,dc=newerahpc,dc=com"
   dn_queue  = "ou=people_queue,dc=newerahpc,dc=com"
   searchScope = ldap.SCOPE_SUBTREE
   searchFilter = "uid=*"	

   ldap_result_id = ldap_conn.search(dn_queue, searchScope, searchFilter, None);
   result_set = []
   while 1:
      result_type, result_data = ldap_conn.result(ldap_result_id, 0)
      if(result_data == []):
         break
      else:
         if result_type == ldap.RES_SEARCH_ENTRY:
            result_set.append(result_data)
	    home_directory = result_data[0][1]['homeDirectory'][0]
	    gid_number = int(result_data[0][1]['gidNumber'][0])
	    uid        = int(result_data[0][1]['uidNumber'][0])
	    if not os.path.isdir(home_directory):
	       os.mkdir(home_directory)
               try:
	          os.chown(home_directory,uid,gid_number)
		  os.chmod(home_directory,0700)
               except OSError:
	 	  print "error"
	    dest_dn = "uid=" + result_data[0][1]['uid'][0] + "," + dn_people
            attrs = result_data[0][1]
	    ldif = modlist.addModlist(attrs)
	    ldap_conn.add_s(dest_dn,ldif)
	    dest_dn = "uid=" + result_data[0][1]['uid'][0] + "," + dn_queue
	    ldap_conn.delete_s(dest_dn)

   cntr = 0
   while cntr < len(result_set):
      cntr += 1

except ldap.LDAPError, ldap_connect_error:
   print ldap_connect_error
   exit





