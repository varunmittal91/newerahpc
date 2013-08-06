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
   function connect_ftp($user_name, $user_passwd)
   {
      $ftp_conn = ftp_connect("localhost") or die ("Ftp server down");
      ftp_login($ftp_conn, $user_name, $user_passwd) or die ("Ftp login failed"); 
      return $ftp_conn;
   }
   function close_ftp($ftp_conn)
   {
      ftp_close($ftp_conn);
   }
   function list_ftp($ftp_conn, $dir)
   {
      $list = ftp_nlist($ftp_conn, $dir);
      return $list;
   }
   function is_dir_ftp($ftp_conn, $curr_dir) 
   {
      if(@ftp_chdir($ftp_conn, "$curr_dir")) 
      {
         @ftp_chdir($ftp_conn, get_parent_dir($curr_dir));
         return true;
      }
      else
      {
         return false;
      }
   }
   function get_parent_dir($dir)
   {
      $pos = strripos($dir,"/");
      if($pos==true)
      {
         $dir = substr($dir,0,$pos);
      }
      return $dir;
   }
   function get_file_name($dir)
   {
      $pos = strripos($dir,"/");
      if($pos!=0)
      {
         $dir = substr($dir,$pos+1,strlen($dir));
      }
      return $dir;
   }
   function create_tmpfile()
   {
      $cntr=0;
      while(1) 
      {
         $file_name = "/tmp/php_$cntr";
         if(is_file($file_name)==false) 
         {
            break;
         }
      $cntr++;
      }
      system("touch $file_name");
      return $file_name;
   }
?>
