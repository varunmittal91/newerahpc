bin_PROGRAMS 	    = rbtree_test thread_test tree_test cfg_test


rbtree_test_SOURCES = rbtree_test.cpp
rbtree_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

thread_test_SOURCES = thread_test.cpp
thread_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

tree_test_SOURCES = thread_test.cpp
tree_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

cfg_test_SOURCES = thread_test.cpp
cfg_test_LDADD   = ../../src/libneweraHPC.la $(INITLIBS)

../../src/libneweraHPC.la:
	make -C "../../src/"
