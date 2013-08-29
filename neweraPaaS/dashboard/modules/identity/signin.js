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

function custom_script_function() {
	$('#signin_form').submit(false);
	$('#submit_signin').click(function(){
		var validator = $('#signin_form').validate({
 		rules: {
			dash_user: {
				required : 1,
				minlength: 5,
				maxlength: 16
			},
			dash_passwd: {
				required:  1,
				minlength: 5,
				maxlength: 16
			}
		},
		errorElement: 'span',
		messages: {
			dash_user: {
				required:  ' Username required',
				minlength: ' Minimum length should be 5'
			},
			dash_passwd: {
				required:  ' Password required', 
				minlength: ' Minimum length should be 5'
			}
		}
		});
		if($('#signin_form').valid()){
			var dash_user   = $("#dash_user").val();
			var dash_passwd = $("#dash_passwd").val();
			var action_data = {'dash_user': dash_user, 'dash_passwd': dash_passwd};
			value = core_perform_action("identity", "check_signin", action_data);
			if(value == 1) {
				$("#error_signin_form").fadeIn();
				console.log("login successful");
				paas_core_refresh_content("home");
			} else {
				$("#error_signin_form").fadeIn();
				console.log(value);				
			}
		}
	});
}
