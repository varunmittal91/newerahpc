all:

linux-plugin:
	gcc -shared -Wl,-soname,libsample_plugin.so.1 -o libsample_plugin.so.1 sample_plugin.cpp -I../

darwin-plugin:
	gcc -dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libsample_plugin.so -o libsample_plugin.1.so sample_plugin.cpp -I../

clean:
	rm -rf libsample_plugin*
	
