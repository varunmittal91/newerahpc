<!DOCTYPE html>
<!--[if lt IE 7]>      <html class="no-js lt-ie9 lt-ie8 lt-ie7"> <![endif]-->
<!--[if IE 7]>         <html class="no-js lt-ie9 lt-ie8"> <![endif]-->
<!--[if IE 8]>         <html class="no-js lt-ie9"> <![endif]-->
<!--[if gt IE 8]><!--> 
<html class="no-js"> <!--<![endif]-->
	<head>
		<meta charset="utf-8">
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
		<title><?php echo $site_title ?></title>
		<meta name="description" content="">
		<meta name="viewport" content="width=device-width">

		<link rel="stylesheet" href="themes/initializr/css/bootstrap.min.css">
		<style>
      	body {
         	padding-top: 60px;
            padding-bottom: 40px;
        }
      </style>
      <link rel="stylesheet" href="themes/initializr/css/bootstrap-responsive.min.css">
      <link rel="stylesheet" href="themes/initializr/css/main.css">

      <script src="themes/initializr/js/vendor/modernizr-2.6.2-respond-1.1.0.min.js"></script>
      <script type="text/javascript" src="include/js/jquery-1.9.0.js"></script>
      <script type="text/javascript" src="include/js/core.js"></script>
      <?php print $script ?>
	</head>
   <body>   
   	<div id="script-area"></div>
   	<div class="navbar navbar-inverse navbar-fixed-top">
      	<div class="navbar-inner">
         	<div class="container">
               <a class="brand" href="#"><?php print $site_title ?></a>
               <div class="nav-collapse collapse">
	               <ul class="nav">
	               	<?php echo $menu ?>
	               	<div id="menu-area"></div>
                     <li class="dropdown">
                     	<a href="#" class="dropdown-toggle" data-toggle="dropdown">Dropdown <b class="caret"></b></a>
                        <ul class="dropdown-menu">
                        	<li><a href="#">Action</a></li>
                           <li><a href="#">Another action</a></li>
                           <li><a href="#">Something else here</a></li>
                           <li class="divider"></li>
                           <li class="nav-header">Nav header</li>
                           <li><a href="#">Separated link</a></li>
                           <li><a href="#">One more separated link</a></li>
                      	</ul>
                 		</li>
                	</ul>
              	</div><!--/.nav-collapse -->
          	</div>
       	</div>
     	</div>

     	<div class="container">
      <!-- Main hero unit for a primary marketing message or call to action -->
      	<div class="row">
         	<div class="span4">
            	<div class='sidebar-nav'>
            		<div class="well">
	            		<div id='content-sidebar'></div>
	            	</div>
              	</div>
          	</div>
	    		<div class="span8">
	    			<div id="content-area"></div>
	      		<?php print $content ?>
	      	</div>
	 		</div>
      	<hr>
      		<footer>
         		<p>&copy; <?php print $site_footer ?></p>
        		</footer>
   	</div> <!-- /container -->


      <script src="themes/initializr/js/vendor/bootstrap.min.js"></script>
      <script src="themes/initializr/js/main.js"></script>
	</body>
</html>
