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

console.log("script loaded");	
inst_stage = core_perform_action('install', 'install_stage');
if(!inst_stage)
	return;
	
$("#stage_1").submit(false);
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

/*
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
*/

function check_install_stage() {
}