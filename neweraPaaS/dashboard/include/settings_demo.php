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
?>

<?php

   $site_title  = "NeweraPaaS Cloud";
   $site_footer = "Varun Mittal";

   $db_agent = "mysql";
   $db_host  = "localhost";
   $db_name  = "neweraPaaS";
   $db_user  = "neweraPaaS";
   $db_pass  = "test123";

   $theme_active = "initializr";

   $modules = array(
                     "home",
                     "identity"
                   );

   $user_roles = array(
                        "admin",
                        "project_admin",
                        "user"
                      );

?>

