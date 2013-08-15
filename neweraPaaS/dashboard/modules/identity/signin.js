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
