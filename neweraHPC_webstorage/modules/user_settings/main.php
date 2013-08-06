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
   function send_load_user_settings(){
      if(isset($_SESSION['logged_in'])){
	 return true;
      }
   }
   function send_menu_user_settings(){
      return "<a href=?q=user_settings>Settings</a><br>";
   }
   function send_content_user_settings(){
      global $ssl_local;
      if(!isset($_GET['q']))return;
      else if($_GET['q']!="user_settings"&&$_GET['q']!="user_settings_save")return;
      else if($_SERVER['SERVER_PORT'] != 443 && $_GET['q']!="user_settings_save") {
	 ssl_set_redirect();
	 ssl_redirect();
      }               
      else if($_GET['q']=="user_settings_save"){
	 /* do somethings */
	 ssl_unset_redirect();
	 ssl_redirect_location($_SERVER['PHP_SELF']);
	 exit();
      } 
      return "<form name=\"user_settings\" action=\"?q=user_settings_save\" method=\"post\">
      Password: <input type=\"password\" name=\"user_pass\" />
      <input type=\"submit\" value=\"Submit\" />
      </form>";
   }
   function send_content_other_user_settings(){
      
   }
?>
