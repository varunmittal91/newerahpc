bin_PROGRAMS 	    = rbtree_test thread_test tree_test cfg_test strings_test rb_queue


rbtree_test_SOURCES = rbtree_test.cpp
rbtree_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

thread_test_SOURCES = thread_test.cpp
thread_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

tree_test_SOURCES = thread_test.cpp
tree_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

cfg_test_SOURCES = cfg_test.cpp
cfg_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

strings_test_SOURCES = strings_test.cpp
strings_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

rb_queue_SOURCES = rb_queue.cpp
rb_queue_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

../../src/libneweraHPC.la:
	make -C "../../src/"
