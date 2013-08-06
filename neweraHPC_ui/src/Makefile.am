SUBDIRS = apps webui_core

lib_LTLIBRARIES            = libneweraHPC_ui.la
libneweraHPC_ui_la_SOURCES = SOURCES
libneweraHPC_ui_la_LIBADD  = apps/libapps.la webui_core/libwebui_core.la
libneweraHPC_ui_la_LDFLAGS = -version-info 2:90 -ldl -lpthread -lneweraHPC
libneweraHPC_ui_la_CFLAGS  = $(AM_CFLAGS)
