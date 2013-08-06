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
   function send_load_register(){
      if(!isset($_SESSION['logged_in']))
         return true;
   }
   function send_menu_register(){
      return "<a href=?q=register>Register</a><br>";
   }
   function send_content_register(){
      if(!isset($_GET['q']))return;
      if($_GET['q']=="register")
      {
	 $content = "
	 <table>
         <form name=\"user_register\" action=\"?q=register_action\" method=\"post\">
	 <tr><td>*First Name:</td> <td><input type=\"text\" name=\"user_first_name\" /></td></tr>
         <tr><td>Last Name:</td> <td><input type=\"text\" name=\"user_last_name\" ></td></tr>
         <tr><td>*Username:</td> <td><input type=\"text\" name=\"user_name\" /></td></tr>
         <tr><td>*Password:</td> <td><input type=\"password\" name=\"user_pass\" /></td></tr>
         <tr><td><input type=\"submit\" value=\"Submit\" /></td></tr>
         </form>
         </table>
	 * items are neccessary.
         ";
	 return $content;
      }
      else if($_GET['q']=="register_action")
      {
         if($_POST['user_first_name']==""|$_POST['user_name']==""||$_POST['user_pass']=="")
            return "Please enter the required data";

         $user_name       = ldap_db_protect($_POST['user_name']);
         $user_first_name = $_POST['user_first_name'];
         $user_last_name  = $_POST['user_last_name'];
	 $user_pass       = $_POST['user_pass'];

	 global $ldap_user_list_queue;
	 global $ldap_base;
         global $home_base_dir;
	 global $ldap_base_group;

         $result = get_login_details(array("uid"),array("$user_name"));
         if($result["count"]==1){
            $content = "Sorry username $user_name is already registered by ".$result[0]['cn'][0];
	    return $content;
         }

         //Using temprory gid from settings.php
         $gidNumber = $ldap_base_group;

         $result           = get_login_details(array("gidNumber"),array($gidNumber));
         $uid_number_count = $result["count"];


	 $crypted_passwd = crypt($user_pass);

	 $dn                          = "uid=$user_name,ou=$ldap_user_list_queue,$ldap_base";
	 $add_array["cn"]             = "$user_first_name $user_last_name";
	 $add_array["uid"]            = $user_name;
	 $add_array["uidNumber"]      = $gidNumber+$uid_number_count+1;
	 $add_array["gidNumber"]      = $gidNumber;
         $add_array["userPassword"]   = "{crypt}$crypted_passwd";
         $add_array["gecos"]          = "$user_first_name $user_last_name";
	 $add_array["homeDirectory"]  = "$home_base_dir/$user_name";
	 $add_array["loginShell"]     = "/bin/bash";
	 $add_array["objectClass"][0] = "account";
	 $add_array["objectClass"][1] = "posixAccount";
	 $add_array["objectClass"][2] = "top";
	 $add_array["objectClass"][3] = "shadowAccount";

	 $content = "Account create request registered<br>Your account will be functional within a minute"; 

	 ldap_db_add($dn,$add_array);
	 return $content;
      }
   }
   function send_content_other_register(){
      
   }
   function get_login_details($attributes,$values) //$attributes and $values are arrays
   {
      $filter = "";
      for($cntr=0;$cntr<count($attributes);$cntr++)
      {
         if($cntr==0)$filter = $filter."(";
         $filter = $filter."$attributes[$cntr]=$values[$cntr]";
         if($cntr==count($attributes)-1)$filter = $filter.")";
      }
      $params  = array("cn");
      $query[0] = $filter;
      $query[1] = $params;
      $result  = ldap_db_query($query);
      return $result;
   }
?>
