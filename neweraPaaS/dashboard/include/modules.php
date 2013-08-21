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

define("ARG_TYPE_POST", 0);
define("ARG_TYPE_GET", 1);
define("ARG_TYPE_SESSION", 2);

function get_default_module() {
	global $modules;
	return $modules[0];
}

function _get_module_path($module) {
	$module_path = "modules/$module/main.php";
	if(file_exists($module_path))
		return $module_path;	
}

function _get_menu_module() {
	global $modules;
	$menu = "";
	$module_path;
	$func_ptr;
	foreach($modules as $module) {
		if(!($module_path = _get_module_path($module))) {
			print "module not found";
			exit(0);
		}
		$func_ptr = $module."_load_menu";
		$menu .= $func_ptr();
	} 	

	print $menu;
}

function _get_sidebar_module() {
	global $modules;
	$sidebar = "";
	$module_path;
	$func_ptr;
	foreach($modules as $module) {
		if(!($module_path = _get_module_path($module))) {
			exit(0);	
		}	
		$func_ptr = $module."_load_sidebar";
		$sidebar .= $func_ptr();
	}	
	
	print $sidebar;
}

function _get_script_module($module) {
	$module_path;
	if(!($module_path = _get_module_path($module)))
		return;
	include_once($module_path);
	$func_ptr = $module."_load_script";
	print $func_ptr();
}

function load_modules() {
   
   global $site_title;
   global $site_footer;

   global $modules;

   $menu    = "";
   $content = "";
   $sidebar = "";
   $script  = "";

	$check_arg_var;
	$module;
	if(($check_arg_var = check_arg('q', 1))) {
		if(!($module = check_arg('module', 1))) {
			if($check_arg_var == 'action' || $check_arg_var == 'script') {
				print 0;
				return;	
			} else {
				if(!($module = get_default_module())) {
					print "404";
					return;	
				}
			}
		}		
		
		$module_path = "modules/$module/main.php";
		if(!file_exists($module_path)) {
			print "404";
			return;
		}
		include_once($module_path);

		$func_ptr = $module;
		if($check_arg_var == 'action') {
			$func_ptr .= '_load_action';
		} else if($check_arg_var == 'get_current_module') {
			print $module;
			return;
		} else if($check_arg_var == 'content') {
			$func_ptr .= '_load_content';
		} else if($check_arg_var == 'sidebar') {
			return _get_sidebar_module();
		} else if($check_arg_var == 'menu') {
			return _get_menu_module();		
		} else if($check_arg_var == 'script') {
			$func_ptr .= '_load_script';	
		} else {
			print "Invalid request";
			return;	
		}
		if(($result_data = $func_ptr()))
			print $result_data;
		else 
			print "404";
		return;
	}	else {
	   global $theme_active;
   	$theme_path = "themes/".$theme_active."/main.php";
   	include_once($theme_path);
		return;
	}
	
	return;

   foreach($modules as $i => $module) {
      $module_path = '';
      if(($pos = strpos($module, '.'))) {
         $mainmodule  = substr($module, 0, $pos);
         $submodule   = substr($module, $pos + 1, strlen($module));

         $module_name = $mainmodule."_".$submodule;
         $module_path = "modules/" . $mainmodule  . "/" . $submodule . ".php";
         $module = $module_name;
      } else {
         $module_path = "modules/" . $module . "/main.php";
      }
      include_once($module_path);

      $func_test_enable = $module."_test_enable";
      if(!$func_test_enable())
         continue;

      $func_load_menu = $module."_load_menu";
      $func_load_content = $module."_load_content";
      $func_load_sidebar = $module."_load_sidebar";
      $func_load_script  = $module."_load_script";

      $menu    .= "<li class='main_menu'>".$func_load_menu()."</li>";
      $content .= $func_load_content();
      $sidebar .= $func_load_sidebar();

      if(function_exists($func_load_script))
         $script .= $func_load_script();
   }

   global $theme_active;

   $theme_path = "themes/".$theme_active."/main.php";
   include_once($theme_path);
}

function set_arg($argument, $arg_type, &$arg_value = NULL) {
   $target = NULL;

   switch($arg_type) {
   case ARG_TYPE_GET:
		$_GET[$argument] = $arg_value;
      break;
   case ARG_TYPE_POST: 
		$_POST[$argument] = $arg_value;
   	break;
   case ARG_TYPE_SESSION:
		$_SESSION[$argument] = $arg_value;
   	break;
   }
}

function check_arg($argument, $arg_type, &$arg_is_empty = NULL) {
   $arg_value = NULL;
   
   switch($arg_type) {
   case ARG_TYPE_GET:
      if(isset($_GET[$argument])) {
         $arg_value = $_GET[$argument]; 
         if(empty($_GET[$argument]))
            $arg_is_empty = 1;
      }
      break;
   case ARG_TYPE_POST:
      if(isset($_POST[$argument])) {
         $arg_value = $_POST[$argument];
         if($arg_is_empty && empty($arg))
            $arg_is_empty = 1;
      }
      break;
   case ARG_TYPE_SESSION:
      if(isset($_SESSION[$argument])) {
         $arg_value = $_SESSION[$argument];
         if($arg_is_empty && empty($arg))
            $arg_is_empty = 1;
      }                             
      break;
   }   

   return $arg_value;
}

?>
