var NuiSpaces = [];

function jsonPath() {
   xml_path = window.location.pathname + ".json";
   return xml_path;
}

function getAppJSON()
{
   var items_fetched;
   
   $.getJSON(jsonPath(), function(items) {
	     items_fetched = items;
	     });
   
   return items_fetched;
}

function sendEventDetails(event)
{
   var response = new Object();
   response.type = "event";
   response.event = new Object();
   response.event.type = "click";
   response.event.child_id = event.target.id;
   
   $.ajax({
	  type: "POST",
	  url: window.location.pathname,
	  data: JSON.stringify(response),
	  contentType: "application/json; charset=utf-8",
	  dataType: "json",
	  success: function(data){alert(data);},
	  failure: function(errMsg) {
	  alert(errMsg);
	  }
	  });
}

var NuiDesktop = function($, window, document, undefined) {
   return {
      
   start: function() {
      
      var items = getAppJSON();
      
      var appName = items.appname;
      var appType = items.app_attributes.apptype;
      
      if(appType != "NuiDesktop")
      {
	 console.log("no desktop requested");
      }
      
      var NuiSpace_id = NuiSpaces.length + 1;
      
      if(NuiSpaces.length == 0)
      {
	 NuiSpaces[0] = NuiDesktop;
      }
      
      var newSpaceDiv = $('<div class="abs" id="desktop">');
      $('#wrapper').append(newSpaceDiv);
      
      NuiDesktop.load_icons(items.NuiDesktop.icon_grid);
      
      for(var i in NuiDesktop.init) {
	 NuiDesktop.init[i]();
      }			
   },
   init: {
   desktop: function() {
      var d = $(document);
      
      d.on('contextmenu', function(event) {
	   }),
      
      d.on('mousedown',	function(event) {
	   if(event.button == 0)
	   {
	   NuiDesktop.handle_left_click(event);
	   }
	   });
   },			
   topBar: function() {
      var newSpaceDiv = $('<div class="abs" id="bar_top">');
      $('#wrapper').append(newSpaceDiv);
   },
   dock: function() {
      var newSpaceDiv = $('<div class="abs" id="bar_bottom">');
      var fullscreenButton = $('<div class="float_right"  id="event_NuiFullscreen">');
      $('#wrapper').append(newSpaceDiv);	
      $('#bar_bottom').append(fullscreenButton);
      $('#event_NuiFullscreen').append('Fullscreen');
   },
   wallpaper: function() {
      $('#wrapper').prepend('<img id="wallpaper" class="abs" src="images/misc/wallpaper.jpg" />');
   }
   },
   load_icons: function(icon_grid) {
      var icon_array = icon_grid.data;
      var icon_count = icon_array.length;
      
      var appName;
      var appLink;
      var iconLink;
      var imgLink;
      var topMargin = 20;
      for(var i=0;i<icon_count;i++)
      {
	 appName  = icon_array[i].id;
	 appLink  = "#_app_" + appName;
	 imgLink  = '<img src="images/icons/icon_32_' + appName + '.png" id="'+appLink+'"/>';
	 iconLink = '<div class="abs icon" style="left:20px;top:'+topMargin+'px;" id="'+appLink+'">'+imgLink+appName+'</div>';
	 $('#desktop').append(iconLink);
	 topMargin += 80;
      }
   },
   handle_right_click: function()
      {
	 
      },
   handle_left_click: function(event)
      {
	 var target_id = event.target.id;
	 if(target_id == "event_NuiFullscreen")
	 {
	    var element = document.getElementById('wrapper');
	    
	    if (element.mozRequestFullScreen) {
	       element.mozRequestFullScreen();
	    } 
	    else if (element.webkitRequestFullScreen) {
	       element.webkitRequestFullScreen();
	    }
	 }
	 else
	 {
	    sendEventDetails(event);
	 }
      },
   window_load_test: function() {
      var window = $("#window");
      
      if (!window.data("kendoWindow")) {
	 window.kendoWindow({
			    height: "200px",
			    width: "200px",
			    title: "About Alvar Aalto",
			    close: 0
			    });
      }
   }
   }
}(jQuery, this, this.document);

$(document).ready(function() {
		  jQuery.ajaxSetup({async:false});
		  NuiDesktop.start();
		  });
