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

function check_logged_in() {
   if(isset($_SESSION['logged_in']))
      return true;
   else 
      return false;
}

function set_logged_in($uid, $gid) {
   $_SESSION['uid'] = $uid;
   $_SESSION['gid'] = $gid;
   $_SESSION['logged_in'] = 1;
}

function add_user($dash_user, $dash_passwd, $rsa_key = NULL) {
	if($rsa_key)
		$fileds = "(_uid, _user, _passwd, _rsa_pub_key) values(@, '$dash_user', '$dash_passwd', '$rsa_key')";
	else 
		$fileds = "(_uid, _user, _passwd) values(@, '$dash_user', '$dash_passwd')";
	   
	$query = "insert into $user $fileds";
	if(query_db($query))
		return 1;
	else return 0;	
} 

?>