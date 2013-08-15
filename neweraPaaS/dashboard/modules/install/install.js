$(document).ready(function() {
	console.log("script loaded");	
   inst_stage = core_perform_action('install', 'install_stage');
   if(!inst_stage)
   	return;

  	$('#stage_1').submit(false);
   $("#submit_stage_1").click(function(){
   	$("#stage_1").validate({
   	rules: {
   		mysql_addr: "required",
   		mysql_port: "required",
   		mysql_user: "required",
   		mysql_passwd: "required",
   	},
   	errorElement: "span",
   	messages: {
   		mysql_addr: "Please enter server address",
   		mysql_port: "Please enter server port",
   		mysql_user: "Please enter admin username",
   		mysql_passwd: "Please enter admin password"
   	}
   	});   	
   	console.log("submit1 stage presed");
   });
   	
   console.log('#stage_' + inst_stage);
   $('#stage_' + inst_stage).fadeIn();
});

function check_install_stage() {
}