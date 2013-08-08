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

function identity_load_script() {
   $test_value = check_arg('q', 1);
   if($test_value && $test_value == 'identity_signup') {

      $script = "<script type='text/javascript' src='http://ajax.googleapis.com/ajax/libs/jquery/1.3.2/jquery.js'></script>
                 <script type='text/javascript'>
                    $(document).ready(function(){
                       $('#login_name').keyup(username_check);
                          $('#tick').hide();
                          $('#cross').hide();
                          $('#login_name_error_len').hide();
			  $('#login_name_error_navail').hide();
                    });
	
                    function username_check(){	
                       console.log('keypressed');
                       var username = $('#login_name').val();
                       if(username == '' || username.length < 4 || username.length > 16) {
                          $('#username').css('border', '3px #CCC solid');
                          $('#tick').hide();
                          $('#cross').fadeIn();
                          $('#login_name').css('border', '3px #C33 solid');
                          $('#login_name_error_len').fadeIn();
                       } else {
                          $('#login_name_error_len').hide();
                          jQuery.ajax({
                             type: 'POST',
                             url: 'modules/identity/check_login_name.php',
                             data: 'username='+ username,
                             cache: false,
                             success: function(response){
                                console.log(response);
                                if(response == 1){
                                   console.log('succss');
       	                           $('#login_name').css('border', '3px #090 solid');	
	                           $('#cross').hide();
	                           $('#tick').fadeIn();
	                        } else {
                                   console.log('failure');
	                           $('#login_name').css('border', '3px #C33 solid');
              	                   $('#tick').hide();
     	                           $('#cross').fadeIn();
                                   $('#login_name_error_navail').show();
	                        }
                             }
                          });
                       }
                    }
                 </script>
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
               <td>Password:</td><td><input type='password' name='login_password'></td>
               </tr>
               <tr><td><input type='submit' value='Login'></td>
               </tr></table>
	       <a href='?q=identity_signup'>Signup</a>
              ";
                
      return $form;
   } else if($test_value && $test_value == 'identity_signup') {

      $form = "<h1>Signup</h1><br>
               Signup in process
               <form method='post' action='?q=identity_signup_action'>
               Username:<input type='text' name='login_name' id='login_name'>
               <img id='tick' src='modules/identity/images/tick.png' width='16' height='16'/>
               <img id='cross' src='modules/identity/images/cross.png' width='16' height='16'/>
               <small><p id='login_name_error_len'>Username length should be minimum 4 and maximum 16</p></small>
               <small><p id='login_name_error_navail'>Username not available</p></small>
               </form>
              ";
      return $form;
   }
}

function identity_load_sidebar() {
}

?>
