<?php
   /*
    *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
    *	NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v2
    *
    *	This file is part of NeweraHPC_webstorage.
    *
    *	NeweraHPC_webstorage is free software: you can redistribute it and/or modify
    *	it under the terms of the GNU General Public License as published by
    *	the Free Software Foundation version 2 of the License.
    *
    *	NeweraHPC_webstorage is distributed in the hope that it will be useful,
    *	but WITHOUT ANY WARRANTY; without even the implied warranty of
    *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *	GNU General Public License for more details.
    *
    *	You should have received a copy of the GNU General Public License
    *	along with NeweraHPC_webstorage.  If not, see <http://www.gnu.org/licenses/>.
    */
?>

<?php
   $site_title = "NeweraHPC WebStorage";
   $site_footer = "©Varun Mittal";
   $db_agent = "ldap";
   $db_host = "localhost";
   if($db_agent=="mysql")
   {
      $mysql_db_user = "Your Database Username";
      $mysql_db_pass = "Your Database Password";
      $mysql_db_name = "Your Database Name";
   }
   else if($db_agent=="ldap")
   {
      $ldap_rootdn          = "cn=admin,dc=newerahpc,dc=com";
      $ldap_rootpw          = "password";
      $ldap_base            = "dc=newerahpc,dc=com";
      $ldap_user_list       = "people";
      $ldap_user_list_queue = "people_queue";
      $ldap_group_list      = "group";
      $ldap_base_group      = 10000;
   }
   $home_base_dir = "/opt/ltsp/home";

   $dummy_user = "test";
   $dummy_pass = "test";
   $theme_default_path = "themes";
   $theme_default_name = "/default";
   $modules_default_path = "modules";
   $ssl_redirect = false;
   $modules = array(
		    "login",
		    "home",
		    "register",
		    "file_manager"
		    );
   $rights = array(
		   "guest",
		   "registered",
		   "moderator",
		   "admin"
		   );
?>
