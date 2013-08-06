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
   global $db_agent;
   include("$db_agent.php");

   $function_template = $db_agent."_db_";

   function init_db()
   {
      global $db_agent;
      $db_init_function = "$db_agent"."_db_init";
      $db_init_function();
   }
   function protect_db($input)
   {
      global $function_template;
      $db_function = $function_template."protect";
      $input = $db_function($input);
   }
   function query_db($query)
   {
      global $function_template;
      $db_function = $function_template."query";
      $result = $db_function($query);
   }
?>
