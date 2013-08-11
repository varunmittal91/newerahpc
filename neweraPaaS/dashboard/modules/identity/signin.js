var valid_login_name = 0;
var valid_login_passwd = 0;

$(document).ready(function() {
   $('#username').blur(username_check);
   $('#userpasswd').blur(passwd_check);

   $('#login_button').click(function() {
      console.log('button pressed');
      if(!valid_login_name && !valid_login_passwd) {
         username_check();
         passwd_check();
      } else if(!valid_login_passwd)
         passwd_check();
      else {
         console.log("sending login");
         var userpasswd = $('#userpasswd').val();
         var username   = $('#username').val();
         jQuery.ajax({
            type: 'GET',
            url: '?q=action&module=identity&func=login_user',
            data: {'username': username, 'userpasswd': userpasswd},
            cache: false,
            success: function(response) {
               console.log(response);
               if(response == 1) {
                  window.location = location.pathname;
               }
               else {
                  $('#login_error_failed').fadeIn();
               }
            }
         })
      }
   })
});

function passwd_check() {
   var passwd = $('#userpasswd').val();
   console.log(passwd);
   if(passwd == '') {
      valid_login_passwd = 0;
      $('#userpasswd_error_required').fadeIn();
      $('#userpasswd').css('border', '3px #C33 solid');
   } else {
      valid_login_passwd = 1;
      $('#userpasswd').css('border', '3px #090 solid');
   }
}

function username_check() {
   var username = $('#username').val();
   if(username == '') {
      valid_login_name = 0;
      $('#username_error_required').fadeIn();
      $('#username').css('border', '3px #C33 solid');
   } else {
      valid_login_name = 1;
      $('#username').css('border', '3px #090 solid');
   }
}
