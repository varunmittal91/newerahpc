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

import signal, os

import paas_mysql
import paas_config
import paas_containers

class Daemon:
   def __init__(self, pidfile, stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
      self.stdin  = stdin
      self.stdout = stdout
      self.stderr = stderr
      signal.signal(signal.SIGTERM, signalHandler)
      signal.signal(signal.SIGINT,  signalHandler)
      

   def daemonize(self):
      try:
         pid = os.fork()
         if pid > 0:
            sys.exit(0) 
      except OSError, e:
         sys.stderr.write("fork ")
         

def signalHandler(signum):
   if signum == signal.SIGTERM or signum == signal.SIGINT:
      print "Shuting down server"

def serverInit(cmd_arguments): 
   config_file = cmd_arguments['config-file']
   
   Config   = paas_config.Config(config_file)
   Database = paas_mysql.Database(Config)
   if not Database.connect(Config):
      return
   Containers = paas_containers.Containers()
   if not Containers.configure(Config, Database):
      return
 
   main_daemon = Daemon('/tmp/run.pid')
   return 1
