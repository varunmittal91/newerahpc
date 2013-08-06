function startApp() {
  getResponse(jsonPath());
}

function jsonPath() {
  xml_path = window.location.pathname + ".json";
  return xml_path;
}

function launch_application(app_path) {
  app_path_ui = "/app/" + app_path + ".json";

  $.ajax({
    url: app_path_ui,
    success: function(responseText, textStatus, xhR) {
      getResponse(app_path_ui);
    }
  });
}

function listener(elem, evnt, func) {
  if (elem.addEventListener) elem.addEventListener(evnt, func, false);
  else if (elem.attachEvent) // For IE
  return elem.attachEvent("on" + evnt, func);
}

function onKeyboard() {
  alert("successfully registered event");
}

var getResponse = function(json_path) {
  $.getJSON(json_path, function(items) {

    var appName = items.appname;
    var appType = items.app_attributes.apptype;

    if (appType == "window") {
      var id = "window_" + items.appname;
      alert(id);
      var e = $('<div id=\"window_calculator\" class=\"abs window\"></div>');
      $('#desktop').append(e);
      $('#window_calculator').addClass('window_stack').show();
    }
    else if (appType == "NuiDesktop") {
      var desktopData = items.NuiDesktop;
      var iconGrid = desktopData.icon_grid;
      var icons = iconGrid.data;
      var iconObject;
      var top = 20;
      for (i = 0; i < icons.length; i++) {
        iconObject = icons[i];
        var newAppName = iconObject.id;

        var href_link = "#" + "_dock_" + newAppName;
        var icon_link = '<img src="assets/images/icons/icon_32_' + newAppName + '.png" />';
        var href_string = '<a class="abs icon" style="left:20px;top:' + top + '"; href=' + href_link + '>' + icon_link + iconObject.id + '</a>';
        var e = $(href_string);
        $('#desktop').append(e);

        top += 80;
      }
    }
  });
};