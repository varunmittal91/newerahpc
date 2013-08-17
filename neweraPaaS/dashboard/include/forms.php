<?php
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

function _core_generate_form_elements($form, $elements) {
	$elem = "";
	foreach($elements as $element) {
		if(!(isset($element["type"])))
			continue;
			
		//$elem_tag = "";
		$element_type = $element["type"];		
		switch($element_type) {
		case "button":
			$elem_tag = "<button class='' id='' name=''></button>";
			break;
		case "text":
		case "password":
			$elem_tag = "<input type='$element_type' class='' id='' name='' placeholder=''>";
			break;
		default:
			continue;
		}
		
		$replacements;
		$targets;
		$i = 0;
		foreach($element as $param => $value) {
			if($param == 'type')
				continue;			
			switch($param) {
			case 'label':

				if($element_type == 'button') {
					$targets[$i]      = "><";
					$replacements[$i] = ">$value<";
					$i++;	
				}
				break;
			case 'placeholder':
			case 'maxlength':
				if($param != 'text')
					continue;
			case 'class':
			case 'id':
			case 'name':
				$targets[$i]      = "$param=''";
				$replacements[$i] = "$param='$value'";
				$i++;
			}			
		} 

		$elem_tag = str_replace($targets, $replacements, $elem_tag);
		$elem_tag = "<div class='controls'>$elem_tag</div>";
		if(isset($element['label']) && $element_type != "button")
			$elem_tag = "<label class='control-label'>$element[label]</label>$elem_tag";
		$elem .= "<div class='control-group'>$elem_tag</div>";		
	}
	
	return $elem;
}

function core_generate_form(&$form_params=NULL) {
	$targets;
	$replacements;
	$i = 0;	
	$form = "<form class='' id='' name='' action='' method=''>";
	
	if(isset($form_params["name"])) {
		$targets[$i]      = "name=''";
		$replacements[$i] = "name='$form_params[name]'";
		$i++;
	}
	if(isset($form_params["id"]))	{
		$targets[$i]      = "id=''";
		$replacements[$i] = "id='$form_params[id]'";
		$i++;
	}
	if(isset($form_params["method"])) {
		$targets[$i]      = "method=''";
		$replacements[$i] = "methods='$form_params[method]'";
		$i++;
	}
	if(isset($form_params["action"])) {
		$targets[$i]      = "action=''";
		$replacements[$i] = "action='$form_params[action]'";
		$i++;
	}
	if(isset($form_params["class"])) {
		$targets[$i]      = "class=''";
		$replacements[$i] = "class='$form_params[class]'";
		$i++;
	}
	$form = str_replace($targets, $replacements, $form);
	if(isset($form_params["legend"])) {	
		$form .= "<legend>$form_params[legend]</legend>";
	}
	if(isset($form_params["error"])) {
   	$form .= "<div class='control-group'><label class='error' style='display: none;color: red;'>$form_params[error]</label></div>";
	}
		
	if(isset($form_params["elements"])) {
		$form .= _core_generate_form_elements($form, $form_params["elements"]);
	}	
	
	$form .= "</form>";
	
	return $form; 
}

?>