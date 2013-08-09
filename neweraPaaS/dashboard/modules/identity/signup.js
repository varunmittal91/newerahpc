                    var valid_login_name = 0;
										var valid_login_passwd = 0;
                    $(document).ready(function() {
                       $('#login_name').blur(username_check);
                       $('#login_tick').hide();
                       $('#login_cross').hide();
                       $('#login_name_error_len').hide();
		                   $('#login_name_error_navail').hide();

                       $('#login_passwd').blur(passwd_check);
                       $('#passwd_tick').hide();
                       $('#passwd_cross').hide();
                       $('#passwd_error_len').hide();
                       $('#passwd_error_navail').hide();   
											 
											 $('#signup_submit').click(function(){
											    console.log('button pressed');
													if(!valid_login_name || !valid_login_passwd) {
													   console.log('invalid username password');
													} else {
														 var userpasswd = $('#login_passwd').val();
												 	   var username   = $('#login_name').val();
											 
													   jQuery.ajax({
														    type: 'POST',
																url: '?q=action&module=identity&func=signup',
																data: {'username': username, 'userpasswd': userpasswd},
																cache: false,
																success: function(response){
																   if(response == 1)
																	    console.log('signup successfull');
																	 else
																	    console.log('signup failed');
																}
														 })
													}
											 })
                    });

                    function passwd_check() {
                       var passwd = $('#login_passwd').val();
                       if(passwd == '' || passwd.length < 8) {
                          $('#username').css('border', '3px #CCC solid');
                          $('#passwd_tick').hide();
                          $('#passwd_cross').fadeIn();
                          $('#login_passwd').css('border', '3px #C33 solid');
                          $('#passwd_error_len').fadeIn();
                          valid_login_passwd = 0;
                       } else {
													$('#passwd_error_len').hide();
                          $('#login_passwd').css('border', '3px #090 solid');
                          $('#passwd_tick').fadeIn();
                          $('#passwd_cross').hide();
													valid_login_passwd = 1;
											 }
                    }

                    function username_check(){	
                       var username = $('#login_name').val();
                       if(username == '' || username.length < 4) {
                          $('#username').css('border', '3px #CCC solid');
                          $('#login_tick').hide();
                          $('#login_cross').fadeIn();
                          $('#login_name').css('border', '3px #C33 solid');
                          $('#login_name_error_len').fadeIn();
                          valid_login_name = 0;
                       } else {
                          $('#login_name_error_len').hide();
                          jQuery.ajax({
                             type: 'POST',
                             url: '?q=action&module=identity&func=check_avail_user',
                             data: 'username='+ username,
                             cache: false,
                             success: function(response){
                                if(response == 1){
       	                           $('#login_name').css('border', '3px #090 solid');	
	                           $('#login_cross').hide();
	                           $('#login_tick').fadeIn();
                                   valid_login_name = 1;
	                        } else {
	                           $('#login_name').css('border', '3px #C33 solid');
              	                   $('#login_tick').hide();
     	                           $('#login_cross').fadeIn();
                                   $('#login_name_error_navail').show();
                                   valid_login_name = 0;
	                        }
                             }
                          });
                       }
                    }

