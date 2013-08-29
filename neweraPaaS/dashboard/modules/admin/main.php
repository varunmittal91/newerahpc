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

function admin_test_enable() {
	include_once('include/auth.php');
	if(($_uid = check_logged_in()) && $_uid == 1)
		return 1;
	else 
		return 0;
}

function admin_load_menu() {
	return "<a href='?module=admin'>Admin</a>";
}

function admin_load_content() {
	
}

function admin_load_sidebar() {
	$links[0] = "<a href='?module=admin&func=loadGroups'>Active Groups</a>";
	$links[1] = "<a href='?module=admin&func=loadUsers'>Active Users</a>";
	return $links;
}

function admin_load_action() {
	
}

?>