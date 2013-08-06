var synthesizedDOM;

var NuiInterfaceParser = {
	init: function(interface, appName, id) {
		synthesizedDOM = document.createDocumentFragment();

		var object = interface.interface.object;
		NuiInterfaceParser.evalObject(object);
		/*
		while(object)
		{
			console.log(object);
			object = object.child;
			//if(object.child)
		}
		*/
		
		console.log(object);
		
		return synthesizedDOM;
	},
	evalObject: function(object)
	{
		var className = object.className;
		console.log(className);
		
		switch(className)
		{
			case "NuiWindow":
				alert("NuiWindow");

				var genre = document.createElement('div');
				$(genre).addClass('abs window');
					
				synthesizedDOM.appendChild(genre);

				break;
			default:
				break;
		}
	},
	getDom: function() {
		return synthesizedDom;
	}
};

/*
		var fragment = synthesizedDOM;

		var heading = fragment.appendChild(document.createElement('h1'));
		heading.appendChild(document.createTextNode('Quotes by Yoda'));
		var blockquote = fragment.appendChild(document.createElement('blockquote'));
		var p = blockquote.appendChild(document.createElement('p'));
		p.appendChild(document.createTextNode('Always two there are'));
*/
