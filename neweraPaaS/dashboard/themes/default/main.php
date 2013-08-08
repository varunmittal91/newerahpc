<!--
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
 -->  
 
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
   <head>
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
      <!-- TemplateBeginEditable name="doctitle" -->
      <title><?php echo $site_title ?></title>
      <!-- TemplateEndEditable -->
      <!-- TemplateBeginEditable name="head" -->
      <!-- TemplateEndEditable -->
      <link href="themes/default/threecoldsn.css" rel="stylesheet" type="text/css" />
   </head>
   
   <body class="thrColLiqHdr">
      
      <div id="container">
	 <div id="header">
	    <h1><?php echo $site_title ?></h1>
	    <!-- end #header --></div>
	 <div id="sidebar1">
	    <h3>Menu</h3>
	    <?php echo $menu ?>
	    <!-- end #sidebar1 --></div>
	 <div id="sidebar2">
	    <?php echo $sidebar ?>
	    <!-- end #sidebar2 --></div>
	 <div id="mainContent">
	    <?php echo $content ?>
	    <!-- end #mainContent --></div>
	 <!-- This clearing element should immediately follow the #mainContent div in order to force the #container div to contain all child floats --><br class="clearfloat" />
	 <div id="footer">
	    <p><?php echo $site_footer ?></p>
	    <!-- end #footer --></div>
	 <!-- end #container --></div>
   </body>
</html>
