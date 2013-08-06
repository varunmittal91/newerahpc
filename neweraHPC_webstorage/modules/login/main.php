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
   function send_load_login()
   {
      return true;
   }
   function send_menu_login()
   {

   }
   function send_content_login()
   {

   }
   function send_content_other_login(){
      global $rights;
      global $db_agent;
      global $ldap_base;
      global $ldap_user_list;
      global $ldap_user_list_queue;
      if(isset($_GET['q'])){
	 if($_GET['q']=="login_action"){
	    if(isset($_SESSION['logged_in'])){
	       echo "<script>location.href=\"index.php\"</script>";
	    }
	    if(!isset($_POST['user_name'])||!isset($_POST['user_pass'])){
	       echo "no donut for you";
	       exit(1);
	    }

	    if($db_agent=="mysql")
	    {
	       $user_name = protect_db($_POST['user_name']);
	       $password  = $_POST['user_pass'];
	       $query = "select permissions from login_details where user_name='$user_name' && user_pass='$password'";
	       $result = query_db($query);
	       if(mysql_num_rows($result)==1)
               {
	          $result = mysql_fetch_row($result);
	          $_SESSION['logged_in']="done";
	          $_SESSION['user_name']=$user_name;
	          $_SESSION['permissions']=$rights[$result[0]];
		  $_SESSION['user_passwd']=$password;
	          header("HTTP/1.1 301 Moved Permanently");
	          header("Location: http://$_SERVER[HTTP_HOST]$_SERVER[REQUEST_URI]");
               }
	       else
	       {
	          return "password invalid";
	       }
	    }
	    else if($db_agent=="ldap")
            {
	       $user_name = ldap_db_protect($_POST['user_name']);
	       $user_pass = $_POST['user_pass'];
	       $base_dn = "ou=$ldap_user_list,$ldap_base";
               $filter = "(uid=$user_name)";
               $params  = array("cn","userPassword","homeDirectory");
               $query[0] = $filter;
               $query[1] = $params;
               $result  = ldap_db_query_dn($query, $base_dn);
	       if($result["count"]==0)
                  return "Username does not exist or has not been yet registered<br>Registration takes about a minute.";
	       $password = $result[0]["userpassword"][0];
	       $check = array("{crypt}"=>"");
	       $password = strtr($password,$check);
	       $password_in = crypt($user_pass,$password);
	       if($password==$password_in)
	       {
                  $_SESSION['logged_in']      = "done";
                  $_SESSION['user_name']      = $user_name;
                  $_SESSION['user_passwd']    = $user_pass;
		  $_SESSION['home_directory'] = $result[0]["homedirectory"][0];
	          header("Location: index.php");
               }
	       else
                  return "User/Password Combination invalid";
            }
         }
	 else if($_GET['q']=="logout"){
	    session_destroy();
	    header("Location: index.php");
	 }
      }
      if(isset($_SESSION['logged_in'])){
	 return "Welcome, $_SESSION[user_name]<br><a href=index.php?q=logout>Logout</a>";
      }
      else if(!isset($_SESSION['logged_in'])){
	 //if($_SERVER['SERVER_PORT'] != 443) {
	 //   header("HTTP/1.1 301 Moved Permanently");
	 //   header("Location: https://".$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI']);
	 //   exit();
	 //}
	 return "<form name=\"user_login\" action=\"?q=login_action\" method=\"post\">
	 Username: <input type=\"text\" name=\"user_name\" />
	 Password: <input type=\"password\" name=\"user_pass\" />
	 <input type=\"submit\" value=\"Submit\" />
	 </form>";
      }
      else{
	 exit(0);
      }
   }
?>
