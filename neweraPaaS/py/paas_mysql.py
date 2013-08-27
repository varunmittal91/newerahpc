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

import paas_config
import paas_errors

import MySQLdb

class Database:
   def __init__(self, config):
      self.config = config
   def connect(self, config):
      mysql_port = 3306
      mysql_host = config.getValue('mysql-host')
      if not mysql_host:
         paas_errors.setError(paas_errors.PAAS_EINVAL, "mysql_host")
         return
      else:
         host_parts = mysql_host.split(':', 1)
         if len(host_parts) == 2:
            mysql_host = host_parts[0]
            mysql_port = int(host_parts[1])         
      mysql_user = self.config.getValue('mysql-user')
      if not mysql_user:
         paas_errors.setError(paas_errors.PAAS_EINVAL, "mysql_user")
         return
      mysql_passwd = config.getValue('mysql-passwd')
      if not mysql_passwd:
         paas_errors.setError(paas_errors.PAAS_EINVAL, "mysql_passwd")
         return
      mysql_db = config.getValue('mysql-db')
      if not mysql_db:
         paas_errors.setError(paas_errors.PAAS_EINVAL, "mysql_db")
         return
      try:
         self.connection = MySQLdb.connect(host = mysql_host, port = mysql_port, user = mysql_user, passwd = mysql_passwd, db = mysql_db)
      except MySQLdb.Error,e:
         if e.args[0] == 1045:                                                     
            paas_errors.setError(paas_errors.PAAS_EACCES)
         elif e.args[0] == 2005:
            paas_errors.setError(paas_errors.PAAS_ENOHOST)
         elif e.args[0] == 2002:
            paas_errors.setError(paas_errors.PAAS_ENOMYSQL)
         else:
            print e.args
         return
      return 1
   def queryMysql(cmd_arguments):
      print "performing query"
   
