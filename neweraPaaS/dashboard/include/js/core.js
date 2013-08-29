 /*
  *     (C) 2013 Varun Mittal <varunmittal91@gmail.com>
  *     NeweraHPC_webstorage program is distributed under the terms of the GNU General Public License v3
  *
  *     This file is part of NeweraHPC.
  *
  *     NeweraHPC is free software: you can redistribute it and/or modify
  *     it under the terms of the GNU General Public License as published by
  *     the Free Software Foundation version 2 of the License.
  *
  *     NeweraHPC is distributed in the hope that it will be useful,
  *     but WITHOUT ANY WARRANTY; without even the implied warranty of
  *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *     GNU General Public License for more details.
  *
  *     You should have received a copy of the GNU General Public License
  *     along with NeweraHPC.  If not, see <http://www.gnu.org/licenses/>.
  */

function core_perform_action(module, func, action_data) {
	if(!action_data)
		action_data = {};
   url = '?q=action&module=' + module + '&func=' + func;
	var ajax_response = 0;
	jQuery.ajax({
		async: false, 
		type: 'GET',
		url: url,
 		data: action_data,
		cache: false,
		success: function(response) {
			ajax_response = response;
      	return response;
   	}
	})	
	
	return ajax_response;
}

function _get_core_action(url, action_data) {
	var ajax_response = 0;

	jQuery.ajax({
		async: false,
		type: 'GET',
		url: url,
		data: action_data,
		cache: false,
		success: function(response) {
			ajax_response = response;
		}  
	})
	return ajax_response;
}

function _get_current_module() {
	url = '?q=get_current_module';
	return _get_core_action(url);
}

function _load_content(module, func) {
	var content_area = $("#content-area");
	var url = "?q=content&module=" + module;
	if(func)
		url += ("&func=" + func);
	console.log(url);
	content_area.hide();
	content_area.empty();
	content_area.append(_get_core_action(url));
	content_area.fadeIn();
}

function _load_sidebar() {
	var url = "?q=sidebar";
	$("#content-sidebar").empty();
	$("#content-sidebar").append(_get_core_action(url));
}

function _load_script(module, func) {
	var url = "?q=script&" + "module=" + module;
	if(func)
		url += ("&func=" + func);
	console.log(url);
	var scripts = _get_core_action(url);
	$("body").append(scripts);
}

function _load_menu() {
	var url = "?q=menu";
	$("#menu-area").load(url);
}

function paas_core_refresh_content(module, func) {
	if(!module)
		module = _get_current_module();
	
	_load_content(module, func);
	_load_menu();
	_load_sidebar();
	_load_script(module, func);
}

$(document).ready(function() {
	var $_GET=[];
   window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi,function(a,name,value){$_GET[name]=value;});
	var module = $_GET["module"];
	var func   = $_GET["func"];
	console.log("func:" + func);
	paas_core_refresh_content(module, func);
	
	try {
		custom_script_function();
	} catch(err) {
		console.log("custom script function could not be loaded or failed");
	}
});