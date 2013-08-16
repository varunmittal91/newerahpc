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

function install_test_enable() {
   return 1;
}

function install_load_menu() {
   return NULL;
}

function check_install_stage() {
   $inst_stage = check_arg("stage", ARG_TYPE_SESSION);
   if($inst_stage)
   {
      return $inst_stage;
   } else {
   	return 1;
   }
}

function install_load_action() {
	$enable_debug = 1;
	$test_val = check_arg('func', ARG_TYPE_GET);
	if($test_val == 'install_stage') {	
		return check_install_stage();
	}
	if($test_val == 'check_mysql') {
		$arg_value = 2;
		set_arg("stage", ARG_TYPE_SESSION, $arg_value);
		return 1;
	} else 	
		return 0;
}

function install_load_script() {
	$script = "<script type='text/javascript' src='include/js/jquery.validate.js'></script>
	           <script type='text/javascript' src='modules/install/install.js'></script>";
	return $script;
}

function install_load_content() {
	$inst_stage = check_install_stage();
	if($inst_stage == 1) {
   	$data = "<div class='well'>
   					<legend><h1>NeweraPaaS Installation</h1></legend>
   					<!-- Checking mysql connectivity in first stage -->
   					<form class='form-horizontal' method='POST' id='stage_1' name='stage_1'>
   						<legend>Verify mysql connectivity</legend>
   							<div class='control-group'>
									<center><label class='error' style='display: none;color:red' color='3px #090 solid' id='check_mysql_error'>Mysql connectivity failed</label></center>
   							</div>
   							<div class='control-group'>
   								<label class='control-label'>Host Address</label>
   								<div class='controls'>
   									<input type='text' class='input-xlarge' id='mysql_addr' name='mysql_addr' placeholder='Server address' maxlength=24>
   								</div>
	   						</div>
   							<div class='control-group'>
   								<label class='control-label'>Host Port</label>
   								<div class='controls'>
   									<input type='text' class='input-xlarge' id='mysql_port' name='mysql_port' placeholder='Server port' value=3306 maxlength=5>
   								</div> 
   							</div>
   							<div class='control-group'>
   								<label class='control-label'>Username</label>
	   							<div class='controls'>
   									<input type='text' class='input-xlarge' id='mysql_user' name='mysql_user' placeholder='Username'>
   								</div>
   							</div>
   							<div class='control-group'>
   								<label class='control-label'>Password</label>
   								<div class='controls'>
   									<input type='text' class='input-xlarge' id='mysql_passwd' name='mysql_passwd' placeholder='Password'>
   								</div>
	   						</div>
   							<div class='control-group'>
   								<div class='controls'>
   									<button class='btn btn-success' id='submit_stage_1'>Check Mysql</button>
   								</div>
   							</div>
   						</form>";
   	return $data;
   } else if($inst_stage == 2) {
   	
		return "<h1>Mysql checked</h1>";   	
   }
   						
/*   						
   						
   				<form class='form-horizontal' method='POST' id='stage_2' style='display: none;'>
   					<legend>Mysql verified</legend>
   				</form>
					</div>";
					*/
}

function install_load_sidebar() {
	
	$stages = array("Checking write permission", "Mysql Connection", "Mysql Database Initialization",
						 "Configuring Steps", "Complete");
	
	$inst_stage = check_install_stage();	
	
	$content = "<ul>";
	foreach($stages as $i => $stage) {
		if($inst_stage == ($i + 1)) {
			$content .= "<li><b>$stage</b></li>";
		} else {
			$content .= "<li>$stage</li>";	
		}
	}	
	$content .= "</ul>";
	
	/*
	$content = "<ul>
						<li>Checking write permission</li>
						<li>Mysql Connection</li>
						<li>Mysql Database initialization</l1>
						<li>Configuring Steps</li>
						<li>Complete</li>
					</ul>";
	*/
	return $content;
}

?>
