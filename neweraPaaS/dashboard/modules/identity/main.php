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

function identity_test_enable() {
   return 1;
}

function identity_load_menu() {
   if(!check_logged_in()) {
   	$links[0] = "Signin";
   	$links[1] = "<a href=?module=identity&func=signin>SignIn</a>";
   	$links[2] = "<a href=?module=identity&func=signup>SignUp</a>";  
   	$links[3] = "";
   	$links[4] = "<a href=?module=identity&func=forgot_password>Forgot Password</a>";
      return $links;
   } else {
   	$link = "<a href=?module=identity&func=signout>SignOut</a>";
   	return $link;	
   }
}

function identity_load_action() {
   $test_val = check_arg('func', 1);
   if($test_val == 'check_signin') {
      $username = check_arg('dash_user', ARG_TYPE_GET);
      $passwd   = check_arg('dash_passwd', ARG_TYPE_GET);

      if(!$username || !$passwd) {
         print 0;
         return;
      }
      $epasswd = md5($passwd);
      $query  = "select _uuid,_uid from users where _user='$username' and _passwd='$epasswd'";
      $result = query_db($query);
      if($result && $result->num_rows > 0) {
      	include_once('include/auth.php');
      	$obj = $result->fetch_object();
      	set_logged_in($obj->_uuid, $obj->_uid);
         return 1;
      }
      return 0;
   } else if($test_val == 'register_user') {
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
   $test_value = check_arg('func', 1);
   $script = "<script type='text/javascript' src='include/js/jquery.validate.js'></script>";
   if($test_value == 'signup' || !$test_value) {
      $script .= "<script type='text/javascript' src='modules/identity/signup.js'></script>";
   } else if($test_value == 'signin') {
      $script .= "<script type='text/javascript' src='modules/identity/signin.js'></script>";
   } else if($test_value == 'signout') {
   	$script .= "<script type='text/javascript'>paas_core_refresh_content()</script>";	
   }
	return $script;
}

function identity_load_content() {
	$test_value = check_arg('func', ARG_TYPE_GET);
	if(!$test_value || $test_value == 'signin') {
		$form_params['class']  = "form-horizontal";
		$form_params['id']     = "signin_form";
		$form_params['name']   = "signin_form";
		$form_params['legend'] = "Signin";
		$form_params['error']  = "Cannot verify username or password";
		
		$form_params['elements'][0]['type']        = 'text';
		$form_params['elements'][0]['label']       = 'Username';
		$form_params['elements'][0]['class']       = 'input-xlarge';	
		$form_params['elements'][0]['placeholder'] = 'Username';
		$form_params['elements'][0]['maxlength']   = '16';	
		$form_params['elements'][0]['id']          = 'dash_user';
		$form_params['elements'][0]['name']        = 'dash_user';
		
		$form_params['elements'][1]['type']        = 'password';
		$form_params['elements'][1]['label']       = 'Password';
		$form_params['elements'][1]['class']       = 'input-xlarge';	
		$form_params['elements'][1]['placeholder'] = 'Password';
		$form_params['elements'][1]['maxlength']   = '16';	
		$form_params['elements'][1]['id']          = 'dash_passwd';
		$form_params['elements'][1]['name']        = 'dash_passwd';
		
		$form_params['elements'][2]['class']  = 'btn btn-success';
		$form_params['elements'][2]['label']  = 'Signin';
		$form_params['elements'][2]['type']   = 'button';
		$form_params['elements'][2]['id']     = 'submit_signin';
		$form_params['elements'][2]['name']   = 'submit_signin';
		
		include_once('include/forms.php');
		$form = core_generate_form($form_params);
  	   return "<div class='well'><legend><h1>NeweraPaaS</h1></legend>$form</div>";
	} else if($test_value == 'signup') {
		$form_params['class']  = 'form-horizontal';
		$form_params['id']     = 'signup_form';
		$form_params['legend'] = 'Signup';
		$form_params['error']  = 'Signup failed';
		
		$form_params['elements'][0]['type']        = 'text';
		$form_params['elements'][0]['label']       = 'Username';
		$form_params['elements'][0]['class']       = 'input-xlarge';
		$form_params['elements'][0]['placeholder'] = 'Desired username';
		$form_params['elements'][0]['maxlength']   = '16';
		$form_params['elements'][0]['id']          = 'dash_user';
		$form_params['elements'][0]['name']        = 'dash_user';	

		$form_params['elements'][1]['type']        = 'text';
		$form_params['elements'][1]['label']       = 'Password';
		$form_params['elements'][1]['class']       = 'input-xlarge';
		$form_params['elements'][1]['placeholder'] = 'Desired password';
		$form_params['elements'][1]['maxlength']   = '16';
		$form_params['elements'][1]['id']          = 'dash_passwd';
		$form_params['elements'][1]['name']        = 'dash_passwd';
		
		$form_params['elements'][2]['type']        = 'text';
		$form_params['elements'][2]['label']       = 'Confirm Password';
		$form_params['elements'][2]['class']       = 'input-xlarge';
		$form_params['elements'][2]['placeholder'] = 'Desired password';
		$form_params['elements'][2]['maxlength']   = '16';
		$form_params['elements'][2]['id']          = 'dash_cpasswd';
		$form_params['elements'][2]['name']        = 'dash_cpasswd';	
			
		$form_params['elements'][3]['class']  = 'btn btn-success';
		$form_params['elements'][3]['label']  = 'Signup';
		$form_params['elements'][3]['type']   = 'button';
		$form_params['elements'][3]['id']     = 'signup_button';
		$form_params['elements'][3]['name']   = 'signup_button';
		
		include_once('include/forms.php');
		$form = core_generate_form($form_params);
  	   return "<div class='well'><legend><h1>NeweraPaaS</h1></legend>$form</div>";
   } else if($test_value == 'signout') {
   	session_destroy();	
   }
}

function identity_load_sidebar() {

}

?>