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

function install_test_enable() {
   return 1;
}

function install_load_menu() {
   return NULL;
}

function install_load_content() {
   $data = '
<div class="well">
   <legend>NeweraPaaS Installation</legend>
</div>';
   
   global $ARG_TYPE_POST;
   global $ARG_TYPE_GET;
   global $ARG_TYPE_SESSION;

   $is_empty;
   $inst_stage = check_arg("stage", $ARG_TYPE_SESSION, $is_empty);
   if($inst_stage) {
      print "$value";
   }
   else
      print "starting installation";

   return $data;
}

function install_load_sidebar() {
}

?>
