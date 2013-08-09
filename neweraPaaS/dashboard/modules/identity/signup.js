var valid_login_name = 0;
var valid_login_passwd = 0;

$(document).ready(function() {
   $('#username').blur(username_check);
   $('#passwd').blur(passwd_check);
   $('#confirm_passwd').blur(match_passwd);
										 
   $('#signup_submit').click(function() {
      console.log('button pressed');
      if(!valid_login_name || !valid_login_passwd) {
         console.log('invalid username password');
      } else {
         var userpasswd = $('#passwd').val();
         var username   = $('#username').val();
         jQuery.ajax({
            type: 'GET',
            url: '?q=action&module=identity&func=register_user',
            data: {'username': username, 'userpasswd': userpasswd},
            cache: false,
            success: function(response) {
               if(response == 1) {
                  console.log('signup successfull');
                  $('#signup_form').hide();
                  $('#welcome_message').fadeIn();
               }
               else
                  console.log('signup failed');
            }
         })
      }
   })
});

function passwd_check() {
   console.log("checking password validity");
   var passwd = $('#passwd').val();
   if(passwd == '' || passwd.length < 8) {
      $('#passwd').css('border', '3px #C33 solid');
      $('#passwd_error_len').fadeIn();
      valid_login_passwd = 0;
   } else {
      $('#passwd_error_len').hide();
      $('#passwd').css('border', '3px #090 solid');
      valid_login_passwd = 1;
   }
}

function match_passwd() {
   var passwd  = $('#passwd').val();
   var cpasswd = $('#confirm_passwd').val();

   if(valid_login_passwd == 1 && passwd == cpasswd) {
      $('#passwd').css('border', '3px #090 solid');
      $('#confirm_passwd').css('border', '3px #090 solid');      
      valid_login_passwd = 1;
   } else {
      $('#passwd').css('border', '3px #C33 solid');
      $('#confirm_passwd').css('border', '3px #C33 solid');      
      $('#passwd_error_nmatch').fadeIn();
      valid_login_passwd = 0;
   }
}

function username_check() {	
   var username = $('#username').val();
   if(username == '' || username.length < 5) {
      $('#username').css('border', '3px #C33 solid');
      $('#username_error_len').fadeIn();
      valid_login_name = 0;
   } else {
      $('#username_error_len').hide();
      jQuery.ajax({
         type: 'GET',
         url: '?q=action&module=identity&func=check_avail_user',
         data: 'username='+ username,
         cache: false,
         success: function(response) {
            if(response == 1) {
       	       $('#username').css('border', '3px #090 solid');	
               $('#username_error_exists').hide();
               valid_login_name = 1;
            } else {
               $('#username').css('border', '3px #C33 solid');
               $('#username_error_exists').fadeIn();
               valid_login_name = 0;
            }
         }
      });
   }
}

