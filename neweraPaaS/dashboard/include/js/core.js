function core_perform_action(module, func, action_data) {
	if(!action_data)
		action_data = {};
   url = '?q=action&module=' + module + '&func=' + func;
	var ajax_response = 0;
	console.log(action_data);
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