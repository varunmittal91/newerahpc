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

function identity_test_enable() {
   return 1;
}

function identity_load_menu() {
   if(!check_logged_in())
      return "<a href=?q=identity_login>Login</a>";
   else 
      return "<a href=?q=identity_logout>Logout</a>";
}

function identity_load_action() {
	 $test_val = check_arg('func', 1);
	 if($test_val == NULL)
	    print 0;
		else if($test_val == 'check_avail_user') {
		  print 1;
		} else if($test_val == 'signup') {
		  $username   = check_arg('username', 0);
			$userpasswd = check_arg('userpasswd', 0);

      if(!$username || !$userpasswd)
			   print 0;
			else {
			   print 1;
			}
		} else {
		  print 0;
		}
}

function identity_load_script() {
   $test_value = check_arg('q', 1);
   if($test_value == 'identity_signup') {

      $script = "<script type='text/javascript' src='http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.js'></script>
                 <script type='text/javascript' src='modules/identity/signup.js'></script>
                ";
      return $script;
   }
}

function identity_load_content() {
   $test_value = check_arg('q', 1);
   if($test_value && $test_value == 'identity_login') {
      $form = "<h1>Login</h1><br>
               <table><tr>
               <form method='post' action='?q=identity_login_action'>
               <td>Login:</td><td><input type='text' name='login_name'></td></tr>
               <tr>
               <td>Password:</td><td><input type='password' name='login_passwd'></td>
               </tr>
               <tr><td><input type='submit' value='Login'></td>
               </tr></table>
	       <a href='?q=identity_signup'>Signup</a>
              ";
                
      return $form;
   } else if($test_value && $test_value == 'identity_signup') {

      $form = "<h1>Signup</h1><br>
               Signup in process
               <form action=''>
               <table cellpadding='5'>
               <tr>
               <td>
                  Username:
               </td>
               <td>
                  <input type='text' name='login_name' id='login_name' maxlength=16>
               </td>
               <td>
                  <img id='login_tick' src='modules/identity/images/tick.png' width='20' height='20'/>
                  <img id='login_cross' src='modules/identity/images/cross.png' width='20' height='20'/>
               </td>
               <td>
                  <small><p id='login_name_error_len'>Username length should be minimum 4 and maximum 16</p></small>
                  <small><p id='login_name_error_navail'>Username not available</p></small>
               </td>
               </tr>
               <tr>
               <td>
                  Password:
               </td>
               <td>
                  <input type='text' name='login_passwd' id='login_passwd' maxlength=32>
               </td>
               <td>
                  <img id='passwd_tick' src='modules/identity/images/tick.png' width='20' height='20'/>
                  <img id='passwd_cross' src='modules/identity/images/cross.png' width='20' height='20'/>
               </td>
               <td>
                  <small><p id='passwd_error_len'>Password length should be minimum 8</p></small>
               </td>
               </tr>
							 <tr>
							 <td>
									<input type='button' id='signup_submit' value='Signup'>
							 </td>
							 </tr>
							 </table>
               </form>
              ";
      return $form;
   }
}

function identity_load_sidebar() {
}

?>
