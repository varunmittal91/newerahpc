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
	console.log("script loaded");	
	inst_stage = core_perform_action('install', 'install_stage');
	if(!inst_stage)
		return;
	
	$("#stage_1").submit(false);
	$("#submit_stage_1").click(function(){
		value = core_perform_action("install", "check_stage_1");
		if(value != 1) {
			$("#error_stage_1").fadeIn();
   		console.log(value);
   	} else {
   		console.log("mysql_update_result:" + value);
  			console.log("submit1 stage presed");
  			paas_core_refresh_content("install");
  		}
  	})   	
	
   $("#stage_2").submit(false);
	$("#submit_stage_2").click(function(){
		var validator = $("#stage_2").validate({
   	rules: {
   		mysql_addr: "required",
   		mysql_port: "required",
   		mysql_user: "required"
   	},
   	errorElement: "span",
   	messages: {
   		mysql_addr: "Please enter server address",
   		mysql_port: "Please enter server port",
   		mysql_user: "Please enter admin username",
   	}
   	});
   	if($("#stage_2").valid()){
			var mysql_addr   = $("#mysql_addr").val();
			var mysql_port   = $("#mysql_port").val();
			var mysql_user   = $("#mysql_user").val();
			var mysql_passwd = $("#mysql_passwd").val();   		
   		var action_data = {'mysql_addr': mysql_addr, 'mysql_port': mysql_port, 'mysql_user': mysql_user, 'mysql_passwd': mysql_passwd}; 
   		console.log(action_data);
   		value = core_perform_action("install", "check_stage_2", action_data);
   		if(value != 1) {
   			$("#error_stage_2").fadeIn();
   			console.log(value);
   		} else {
   			console.log("mysql_update_result:" + value);
   			console.log("submit1 stage presed");
   			paas_core_refresh_content("install");
   		}   	
   	}
	});	

	console.log('#stage_' + inst_stage);
}

function check_install_stage() {
}
