<?php
   /*
    *   (C) 2011 Varun Mittal <varunmittal91@gmail.com>
    *   NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v2
    *
    *   This file is part of NeweraHPC_webstorage.
    *
    *   NeweraHPC_webstorage is free software: you can redistribute it and/or modify
    *   it under the terms of the GNU General Public License as published by
    *   the Free Software Foundation version 2 of the License.
    *
    *   NeweraHPC_webstorage is distributed in the hope that it will be useful,
    *   but WITHOUT ANY WARRANTY; without even the implied warranty of
    *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *   GNU General Public License for more details.
    *
    *   You should have received a copy of the GNU General Public License
    *   along with NeweraHPC_webstorage.  If not, see <http://www.gnu.org/licenses/>.
    */
?>

<?php
   function mysql_db_init()
   {
      global $db_user;
      global $db_pass;
      global $db_name;
      global $db_host;
      global $db_name;
      mysql_connect($db_host,$db_user,$db_pass);
      mysql_select($db_name);
   }
   function mysql_db_query($query)
   {
      $result = mysql_query($query);
      return $result;
   }
   function mysql_db_protect($input)
   {
      $input = mysql_real_escape_string($input);
      return $input;
   }
?>
