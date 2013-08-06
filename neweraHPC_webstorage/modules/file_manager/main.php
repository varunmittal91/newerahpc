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
   function send_load_file_manager()
   {
      if(isset($_SESSION['logged_in']))
      {
	 $_SESSION['ftp_conn'] = connect_ftp($_SESSION['user_name'], $_SESSION['user_passwd']);
         return true;
      }
   }
   function send_menu_file_manager()
   {
      return "<a href=?q=file_manager>File Manager</a>";
   }
   function send_content_file_manager()
   {
      $max_column = 4;

      if(!isset($_GET['q']))return;
      if($_GET['q']=="file_manager")
      {
         $ftp_conn = $_SESSION['ftp_conn'];
         $content = "Welcome to file manager<br><table>";

	 $path_type = "directory";

         $home_dir = $_SESSION['home_directory'];
         if(isset($_GET['dir']) && strpos($_GET['dir'],"..")==false)
	 {
	    $child_dir = $_GET['dir'];
	 }
         else
            $child_dir = "";
	 $temp_path = "$home_dir/$child_dir";

         if(is_dir_ftp($ftp_conn, $temp_path)==true)
         {
            $final_path = $temp_path;
         }
         else
         {
            if(ftp_size($ftp_conn, $temp_path)!=-1)
            {
               $final_path = $temp_path;
               $path_type  = "file";
               $file_size  = ftp_size($ftp_conn, $temp_path);
	       $file_name  = get_file_name($final_path);
            }
            else
            {
               $final_path = $home_dir;
               $child_dir  = "";
            }
         }

	 $child_dir = str_replace(" ", "%20", $child_dir);

         if($path_type=="directory")
         {
            $file_list = list_ftp($ftp_conn, $final_path);
            $count     = count($file_list);
            if(isset($_GET['method']) && isset($_FILES['upload_file_name']))
            {
	       if($_FILES['upload_file_name']['error'] == 0)
	       {
                  $upload_file_name = $_FILES['upload_file_name']['name'];
	          $upload_tmp_name  = $_FILES['upload_file_name']['tmp_name'];
                  $upload_dest_path = $final_path."/".$upload_file_name;
	          $upload_dest_path = str_replace(" ", "\ ", $upload_dest_path);
                  ftp_put($ftp_conn, $upload_dest_path, $upload_tmp_name ,FTP_BINARY) or die("Error: Please contact administrator");
	          header("Location: index.php?q=file_manager&dir=$child_dir");
	       }
               else
                  die("Error: Please contact administrator");
            }
         }
         else
         {
	       $tmp_file = create_tmpfile();
	       $ret = ftp_get($ftp_conn, $tmp_file, $final_path, FTP_BINARY);
	       header("Location: modules/file_manager/download.php?q=$child_dir&file=$tmp_file");
         }

         if($path_type=="directory")
         {
            if($child_dir!="")
            {
               $back_link = get_parent_dir($child_dir);
	       if(strripos($child_dir,"/")==0)$back_link="";
               $content = $content."<br><a href=?q=file_manager&dir=$back_link>Back</a><br>";
            }
	    $content = $content."<br>$count Files and Directories in this folder<br>$child_dir<br>";
            $pos_cntr = 0;
            for($cntr=0;$cntr<$count;$cntr++)
            {
               $dir_name = get_file_name($file_list[$cntr]);
               if($pos_cntr==0)
                  $content = $content."<tr>";

	       $short_name = $dir_name;
               if(strlen($short_name)>6)
               {
                  $short_name = substr($short_name,0,4)."..";
               }

               $file_type = is_dir_ftp($ftp_conn, $file_list[$cntr]);
               if($file_type==true)
               {
                  $img_src = "modules/file_manager/images/folder.jpg";
               }
               else
               {
                  $img_src = "modules/file_manager/images/file.jpg";
               }

               $file_link = "$child_dir/$dir_name";
	       $file_link = str_replace(" ", "%20", $file_link);

	       $content = $content."<td><a href=?q=file_manager&dir=$file_link title=$dir_name><img src=$img_src height='20%' width='20%'>$short_name</a></td>";

               if($pos_cntr==($max_column-1))
               {
                  $content = $content."</tr>";
                  $pos_cntr = 0;
               }
               else
                  $pos_cntr++;

            }
            $content = $content."</table><br>";
            $content = $content.
			"<form enctype=multipart/form-data action=?q=file_manager&dir=$child_dir&method=upload method=POST>
			<input type=hidden name=MAX_FILE_SIZE value=10000000 />
			Choose a file to upload: <input type=file name=upload_file_name /><br />
			<input type=submit value=Upload File />
			</form>";
         }
         return $content;
      }
      else if($_GET['q']=="file_manager_upload")
      {
      }
   }
   function send_content_other_file_manager()
   {

   }
   function download_file($file_name)
   {
      header('Content-Description: File Transfer');
      header('Content-Type: application/octet-stream');
      header("Content-Disposition: attachment; filename=" . get_file_name($file_name));
      header('Content-Transfer-Encoding: binary');
      readfile("/tmp/tmp_file");
   }
?>

