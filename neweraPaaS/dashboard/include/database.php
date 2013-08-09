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

$db_conn = NULL;

function init_db() {
   global $db_host;
   global $db_user;
   global $db_pass;
   global $db_name;
   global $db_conn;

   $db_conn = new mysqli($db_host, $db_user, $db_pass, $db_name);
   if($db_conn->connect_errno) {
      echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
      exit(0);
   }
}

function query_db($query) {
   global $db_conn;
   $res = $db_conn->query($query);
   return $res;
}

function fetch_result_db($query, $parameters) {

}

?>
