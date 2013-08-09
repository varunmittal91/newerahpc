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
   else if($test_val == 'register_user') {
      $username = check_arg('username', 1);
      $passwd   = check_arg('userpasswd', 1);
      if(!$username || !$passwd) {
         print 0;
         return;
      }
      $epasswd = md5($passwd);
      $query  = "insert into users(uid, user, passwd) values(@, '$username', '$epasswd')";
      $result = query_db($query);
      if($result)
         print 1;
      else
         print 0;
      return; 
   } else if($test_val == 'check_avail_user') {
      $username   = check_arg('username', 1);
      if(!$username) 
         print 0;
      else {
         $query = "select user from users where user='$username'";
         $res   = query_db($query);
         if(!$res || $res->num_rows > 0)
            print 0;
         else
            print 1;
         return;
      }
   } else {
      print 0;
   }
}

function identity_load_script() {
   $test_value = check_arg('q', 1);
   if($test_value == 'identity_signup') {

      $script = "<script type='text/javascript' src='themes/js/jquery-1.3.2.js'></script>
                 <script type='text/javascript' src='modules/identity/signup.js'></script>
                ";
      return $script;
   }
}

function identity_load_content() {
   $test_value = check_arg('q', 1);
   if($test_value && $test_value == 'identity_login') {  
      $form = "
<div class='well'>
   <form class='form-horizontal' method='POST' action=''>
      <legend>Login</legend>
      <div class='control-group'>
         <label class='control-label'>Username</label>
         <div class='controls'>
            <input type='text' class='input-xlarge' id='username' name='username' placeholder='Username' maxlength=16>
         </div>
      </div>
      <div class='control-group'>
         <label class='control-label'>Password</label>
         <div class='controls'>
            <input type='password' class='input-xlarge' id='userpasswd' name='userpasswd' placeholder='Password' maxlength=16>
         </div>
      </div>
      <div class='control-group'>
         <label class='control-label'></label>
         <div class='controls'>
            <button type='submit' class='btn btn-success'>Login</button>  
         </div>
      </div>
      <hr>
   </form>
   <div class='control-group'>
      <a href='?q=identity_signup'>
         <label class='control-label'>Create New Account</label>
      </a>
   </div>
</div>
";
      return $form;
   } else if($test_value && $test_value == 'identity_signup') {

      $form = "
<div class='well'>
   <legend>Sign Up</legend>
   <form class='form-horizontal' id='signup_form'>
      <div class='control-group'>
         <label class='control-label'>Username</label>
         <div class='controls'>
            <input type='text' class='input-xlarge' id='username' name='username' placeholder='Username' maxlength=16>
            <small><p class='error' style='display: none' id='username_error_len'>Username length should be atleast 5</p></small>
            <small><p class='error' style='display: none' id='username_error_exists'>Username not available</p></small>
         </div>
      </div>
      <div class='control-group'>
         <label class='control-label'>Password</label>
         <div class='controls'>
            <input type='password' class='input-xlarge' id='passwd' name='passwd' placeholder='Password' maxlength=16>
            <small><p class='error' style='display: none' id='passwd_error_nmatch'>Passwords do not match</p></small>
            <small><p class='error' style='display: none' id='passwd_error_len'>Password length should me minimum 8</p></small>
         </div>
      </div>
      <div class='control-group'>
         <label class='control-label'>Confirm Password</label>
         <div class='controls'>
            <input type='password' class='input-xlarge' id='confirm_passwd' name='confirm-passwd' placeholder='Confirm Password' maxlength=16>
         </div>
      </div>
      <div class='control-group'>
         <label class='control-label'></label>
         <div class='controls'>
            <button type='button' class='btn btn-success' id='signup_submit' name='signup_submit'>Signup</button>
         </div>
      </div>
   </form>
   <div id='welcome_message' style='display: none'><h1>Welcome to Cloud</h1></div>
</div>
";
      return $form;
   }
}

function identity_load_sidebar() {
}

?>
