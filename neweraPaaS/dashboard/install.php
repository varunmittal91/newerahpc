<?php
 /*
  *     (C) 2013 Varun Mittal <varunmittal91@gmail.com>
  *     NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v3
  *
  *     This file is part of NeweraHPC.
  *
  *     NeweraHPC is free software: you can redistribute it and/or modify
  *     it under the terms of the GNU General Public License as published by
  *     the Free Software Foundation version 2 of the License.
  *
  *     NeweraHPC is distributed in the hope that it will be useful,
  *     but WITHOUT ANY WARRANTY; without even the implied warranty of
  *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *     GNU General Public License for more details.
  *
  *     You should have received a copy of the GNU General Public License
  *     along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
  */

   if(file_exists('include/settings.php')) {
      echo "It appears dashboard is already installed, remove settings.php from include directory to install again.";
      exit(0);
   }

   include('include/database.php');
   include('include/modules.php');
   include('include/auth.php');

   $site_title  = "NeweraPaaS Cloud Setup";
   $site_footer = "Varun Mittal";

   $modules = array("install");
   $theme_active = "initializr";

   session_start();
   load_modules();
?>