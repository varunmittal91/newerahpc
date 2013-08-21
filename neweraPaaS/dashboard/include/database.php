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

define("DB_PARAMETER_TYPE_COMPARE", "compare");

$db_conn = NULL;

function setup_parameters_db($_db_host, $_db_port, $_db_user, $_db_passwd, $_db_name) {
	global $db_host;
	global $db_port;
	global $db_user;
	global $db_passwd;
	global $db_name;
	
	$db_host   = $_db_host;
	$db_port   = $_db_port;
	$db_user   = $_db_user;
	$db_passwd = $_db_passwd;
	$db_name   = $_db_name;
	
	return 1; 
}

function init_db() {
   global $db_host;
   global $db_user;
   global $db_passwd;
   global $db_name;
   global $db_conn;

   $db_conn = @new mysqli($db_host, $db_user, $db_passwd, $db_name) or die("Error");
   if($db_conn->connect_errno) {
      echo "Failed to connect to MySQL: (" . $db_conn->connect_errno . ") " . $db_conn->connect_error . "<br>";
      exit(0);
   }
   
   return 1;
}

function test_connection_db($db_host, $db_port, $db_user, $db_passwd, $db_name = "") {
	global $db_conn; 
	$db_conn = new mysqli($db_host, $db_user, $db_passwd, $db_name, $db_port);
	if($db_conn && !$db_conn->connect_errno)
		return 1;
	return 0;
}

function insert_db($query) {
	global $db_conn;	
	return $db_conn->real_query($query);
}

function query_db($query) {
   global $db_conn;
   if(!$db_conn) {
   	return 0;	
   } else {
   	$res = $db_conn->query($query);
   	return $res;
   }
}

function fetch_result_db($fields, $table, $parameters) {
   $field_list = NULL;
   $param_list = NULL;

   foreach($fields as $field) {
      if(!$field_list)
         $field_list = $field; 
      else
         $field_list .= ",$field";
   }
   foreach($parameters as $field => $values) {
      $param = _prepare_param($field, $values);
      print "$param<br>";
   }
   //$query = "select $field_list from $table
}

function _param_compare($field, $value) {
   if(!$field || !$value)
      return NULL; 
   return "$field = $value";
}

function _param_and($filed, $value) {
   
}

function _prepare_param($field, $values) {
   $value      = $values[0];
   $param_type = $values[1];
   $func = "_param_" . $param_type;
   if(!function_exists($func))
      return NULL;
   return $func($field, $value);
}

?>
