<?php
      if(!isset($_GET['q'])||!isset($_GET['file']))
      {
         echo "Maybe you are trying to hack the system";
	 exit(1);
      }
      header('Content-Description: File Transfer');
      header('Content-Type: application/octet-stream');
      header("Content-Disposition: attachment; filename=" . $_GET['q']);
      header('Content-Transfer-Encoding: binary');
      readfile($_GET['file']);
      if(strpos($_GET['file'],"/tmp/")==0)
         system("rm $_GET[file]");
?>

