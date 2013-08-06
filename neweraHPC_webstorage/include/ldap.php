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
   
   $ldap_conn;
   
   function ldap_db_init()
   {
      global $ldap_rootdn;
      global $ldap_rootpw;
      global $db_host;
      global $ldap_conn;

      $ldap_conn = ldap_connect($db_host) or die ("Failed to connect to LDAP server");
      ldap_set_option($ldap_conn, LDAP_OPT_PROTOCOL_VERSION, 3);
      $ldap_bind = ldap_bind($ldap_conn,$ldap_rootdn,$ldap_rootpw);
      if(!$ldap_bind)
         die("Failed to bind to LDAP server");
   }
   function ldap_db_query($query)
   {
      global $ldap_conn;
      global $ldap_base;

      $result = ldap_search($ldap_conn, $ldap_base, $query[0], $query[1]) or die("hiii");
      $result = ldap_get_entries($ldap_conn, $result) or die ("hii");
      return $result;
   }
   function ldap_db_query_dn($query, $ldap_base)
   {
      global $ldap_conn;

      $result = ldap_search($ldap_conn, $ldap_base, $query[0], $query[1]) or die("hiii");
      $result = ldap_get_entries($ldap_conn, $result) or die ("hii");
      return $result;
   }
   function ldap_db_add($dn,$input_array)
   {
      global $ldap_conn;
      
      ldap_add($ldap_conn,$dn,$input_array);// or die("Failed to add entry in LDAP server");
   }
   function ldap_db_protect($query)
   {
      $banned_char = array("*"=>"");
      $query = strtr($query,$banned_char);
      return $query;
   }
?>
