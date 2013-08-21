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
	} else if($test_val == 'check_stage_1') {
		if(!(fopen("include/settings.php", "w")))
			return 0;
		else {
			$arg_value = 2;
			set_arg("stage", ARG_TYPE_SESSION, $arg_value);
			return 1;
		}
	} else if($test_val == 'check_stage_2') {
		$db_host   = "";
		$db_port   = "";
		$db_user   = ""; 
		$db_passwd = "";
		if(($db_host = check_arg("mysql_addr", 1)) && ($db_port = check_arg("mysql_port", 1)) && ($db_user = check_arg("mysql_user", 1))) {
			if(!($db_passwd = check_arg("mysql_passwd", 1)))
				$db_passwd = "";
			if(!($db_name = check_arg("mysql_db", 1)))
				$db_name = "neweraPaaS";
			include_once('install_utils.php');
			if(!_create_database($db_host, $db_port, $db_user, $db_passwd, $db_name))
				return 0;			
			
			$arg_value = 3;
			set_arg("db_host", ARG_TYPE_SESSION, $db_host);
			set_arg("db_port", ARG_TYPE_SESSION, $db_port);
			set_arg("db_user", ARG_TYPE_SESSION, $db_user);
			set_arg("db_passwd", ARG_TYPE_SESSION, $db_passwd);
			set_arg("db_name", ARG_TYPE_SESSION, $db_name);
			
			set_arg("stage", ARG_TYPE_SESSION, $arg_value);
			return 1;
		}
		else {
			return 0;
		}
	} else if($test_val == 'check_stage_3') {
		if(!($dash_user = check_arg("dash_user", ARG_TYPE_GET)) || !($dash_passwd = check_arg("dash_passwd", ARG_TYPE_GET))) {
			return 0;
		}

		if(!init_db())
			return 0;

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
	include_once("include/forms.php");

	if($inst_stage == 1) {
		$form_params['class']  = "form-horizontal";
		$form_params['id']     = "stage_1";
		$form_params['legend'] = "Verify write permissions";
		$form_params['error']  = "No write access available in folder 'include'";
		$form_params['method'] = "Post";
		
		$form_params['elements'][0]['type']  = 'button';	
		$form_params['elements'][0]['class'] = 'btn btn-success';
		$form_params['elements'][0]['label'] = 'Check Permissions';
		$form_params['elements'][0]['id']    = 'submit_stage_1';	

		$form = core_generate_form($form_params);
		return "<div class='well'><legend><h1>NeweraPaaS Installation</h1></legend>$form</div>";
	} 	else if($inst_stage == 2) {
		$form_params['class']  = "form-horizontal";
		$form_params['id']     = "stage_2";
		$form_params['legend'] = "Verify mysql connectivity";
		$form_params['error']  = "Mysql Connectivity failed";
		$form_params['method'] = "Post";
		
		$form_params['elements'][0]['type']        = 'text';
		$form_params['elements'][0]['label']       = 'Host address';
		$form_params['elements'][0]['maxlength']   = '24';
		$form_params['elements'][0]['placeholder'] = 'Server address';
		$form_params['elements'][0]['id']          = 'mysql_addr';
		$form_params['elements'][0]['name']        = 'mysql_addr';
		
		$form_params['elements'][1]['type']        = 'text';
		$form_params['elements'][1]['label']       = 'Host port';
		$form_params['elements'][1]['maxlength']   = '5';
		$form_params['elements'][1]['placeholder'] = 'Server port';
		$form_params['elements'][1]['id']          = 'mysql_port';
		$form_params['elements'][1]['name']        = 'mysql_port';
		$form_params['elements'][1]['value']       = '3306';
		
		$form_params['elements'][2]['type']        = 'text';
		$form_params['elements'][2]['label']       = 'Username';
		$form_params['elements'][2]['maxlength']   = '16';
		$form_params['elements'][2]['placeholder'] = 'Database username';
		$form_params['elements'][2]['id']          = 'mysql_user';
		$form_params['elements'][2]['name']        = 'mysql_user';
		
		$form_params['elements'][3]['type']        = 'password';
		$form_params['elements'][3]['label']       = 'Password';
		$form_params['elements'][3]['maxlength']   = '16';
		$form_params['elements'][3]['placeholder'] = 'Database password';
		$form_params['elements'][3]['id']          = 'mysql_passwd';
		$form_params['elements'][3]['name']        = 'mysql_passwd';
		
		$form_params['elements'][4]['type']        = 'text';
		$form_params['elements'][4]['label']       = 'Database name';
		$form_params['elements'][4]['maxlength']   = '16';
		$form_params['elements'][4]['placeholder'] = 'Default neweraPaaS';
		$form_params['elements'][4]['id']          = 'mysql_db';
		$form_params['elements'][4]['name']        = 'mysql_db';
		
		$form_params['elements'][5]['type']  = 'button';	
		$form_params['elements'][5]['class'] = 'btn btn-success';
		$form_params['elements'][5]['label'] = 'Check Permissions';
		$form_params['elements'][5]['id']    = 'submit_stage_2';	
		
		$form = core_generate_form($form_params);
		return "<div class='well'><legend><h1>NeweraPaaS Installation</h1></legend>$form</div>";
   } else if($inst_stage == 3) {   	
		$form_params['class']  = "form-horizontal";
		$form_params['id']     = "stage_3";
		$form_params['legend'] = "Initialize MysqlData";
		$form_params['error']  = "Cannot create database";
		
		$form_params['elements'][0]['type']        = 'text';
		$form_params['elements'][0]['label']       = 'Admin Username';
		$form_params['elements'][0]['class']       = 'input-xlarge';	
		$form_params['elements'][0]['placeholder'] = 'Dashboard admin username';
		$form_params['elements'][0]['maxlength']   = '16';	
		$form_params['elements'][0]['id']          = 'dash_user';
		$form_params['elements'][0]['name']        = 'dash_user';
				
		$form_params['elements'][1]['type']        = 'password';
		$form_params['elements'][1]['label']       = 'Admin Password';
		$form_params['elements'][1]['class']       = 'input-xlarge';
		$form_params['elements'][1]['placeholder'] = 'Dashboard admin password';
		$form_params['elements'][1]['maxlength']   = '16';
		$form_params['elements'][1]['id']          = 'dash_passwd';
		$form_params['elements'][1]['name']        = 'dash_passwd';
		
		$form_params['elements'][2]['type']        = 'text';
		$form_params['elements'][2]['label']       = 'Database Name';
		$form_params['elements'][2]['class']       = 'input-xlarge';
		$form_params['elements'][2]['palceholder'] = 'Deffault paas_dashboard';
		$form_params['elements'][2]['maxlength']   = '16';
		$form_params['elements'][2]['id']          = 'dash_db';
		$form_params['elements'][2]['name']        = 'dash_db';
		
		$form_params['elements'][3]['type']  = 'button';	
		$form_params['elements'][3]['class'] = 'btn btn-success';
		$form_params['elements'][3]['label'] = 'Create Database';
		$form_params['elements'][3]['id']    = 'submit_stage_3';	

		include_once("include/forms.php");
		$form = core_generate_form($form_params);
		return "<div class='well'><legend><h1>NeweraPaaS Installation</h1></legend>$form</div>";
   }
}

function install_load_sidebar() {
	
	$stages = array("Check write permission", "Check Mysql Connection", "Initialize MysqlData",
						 "Configuring Steps", "Complete");
	
	$inst_stage = check_install_stage();	
	
	$content = "<ul class='nav nav-list'><li class='nav-header'>Installation steps</li>";
	foreach($stages as $i => $stage) {
		if($inst_stage == ($i + 1)) {
			$content .= "<b><big><li class='active'>$stage</li></big></b>";
		} else {
			$content .= "<li>$stage</li>";	
		}
	}	
	$content .= "</ul>";
	return $content;
}

?>
