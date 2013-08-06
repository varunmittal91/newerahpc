<?php
   /*
    *	(C) 2011 Varun Mittal <varunmittal91@gmail.com>
    *	NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v2
    *
    *	This file is part of NeweraHPC_webstorage.
    *
    *	NeweraHPC_webstorage is free software: you can redistribute it and/or modify
    *	it under the terms of the GNU General Public License as published by
    *	the Free Software Foundation version 2 of the License.
    *
    *	NeweraHPC_webstorage is distributed in the hope that it will be useful,
    *	but WITHOUT ANY WARRANTY; without even the implied warranty of
    *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *	GNU General Public License for more details.
    *
    *	You should have received a copy of the GNU General Public License
    *	along with NeweraHPC_webstorage.  If not, see <http://www.gnu.org/licenses/>.
    */
?>

<?php
   function load_modules(){
      global $modules;
      global $modules_default_path;
      global $menu;
      global $content;
      global $content_other;
      global $site_title;
      global $site_footer;
      foreach($modules as $i => $value){
	 $path = $modules_default_path."/".$modules[$i]."/main.php";
	 include ($path);
	 $func_load = "send_load_".$modules[$i];
	 $load = $func_load();
	 if($load==false)continue;
	 $func_menu = "send_menu_".$modules[$i];
	 $func_content = "send_content_".$modules[$i];
	 $func_content_other = "send_content_other_".$modules[$i];
	 $menu_tmp = $menu.$func_menu();
	 $menu = $menu.$func_menu();
	 $content = $content.$func_content();
	 $content_other = $content_other.$func_content_other();
      }
      include('theme.php');
   }
?>
