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
?>

<?php

function load_modules() {
   
   global $site_title;
   global $site_footer;

   global $modules;

   $menu    = "";
   $content = "";
   $sidebar = "";
   $script  = "";

   if(check_arg('q', 1) == 'action') {
      $module = check_arg('module', 1);
      if($module == NULL) {
         $_GET['q'] = 'home';
      } else {
         include_once('modules/'.$module.'/main.php');
         $func_load_action = $module."_load_action";
         return $func_load_action();
      }
   }

   foreach($modules as $i => $module) {
      $module_path = "modules/" . $module . "/main.php";
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

function check_arg($argument, $is_get) {
   if($is_get) {
      if(isset($_GET[$argument]))
         return $_GET[$argument];
      else
         return NULL;
   } else {
      if(isset($_POST[$argument]))
         return $_POST[$argument];
      else 
         return NULL;
   } 

   return NULL;
}

?>
