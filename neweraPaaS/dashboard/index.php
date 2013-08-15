<?php
 /*
  *	(C) 2013 Varun Mittal <varunmittal91@gmail.com>
  *	NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v3
  *
  *	This file is part of NeweraHPC.
  *
  *	NeweraHPC is free software: you can redistribute it and/or modify
  *	it under the terms of the GNU General Public License as published by
  *	the Free Software Foundation version 2 of the License.
  *
  *	NeweraHPC is distributed in the hope that it will be useful,
  *	but WITHOUT ANY WARRANTY; without even the implied warranty of
  *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *	GNU General Public License for more details.
  *
  *	You should have received a copy of the GNU General Public License
  *	along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
  */
?>

<?php
   include_once('include/boot.php');
   if(!check_installed()) {
      print "Dashboard is not installed, use manual to install and configure";
      exit(0);
   }

   include_once('include/settings.php');
   include_once('include/database.php');
   include_once('include/modules.php');
   include_once('include/auth.php');

   boot();
?>
