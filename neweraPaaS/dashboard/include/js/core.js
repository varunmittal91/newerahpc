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

function load_content(module, func) {
	var url = "?q=content&module=" + module;
	$("#content-area").load(url);
}

function load_script(module) {
	var url = "?q=script&" + "module=" + module;
	$("#script-area").load(url);
}

function load_menu() {
	var url = "?q=menu";
	$("#menu-area").load(url);
}

$(document).ready(function() {
	var module = _get_current_module();
	load_content(module);
	load_menu();
	load_script(module);
});