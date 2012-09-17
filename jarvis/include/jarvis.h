#ifndef _JARVIS_H_
#define _JARVIS_H_

#include <neweraHPC/thread.h>

#include "general.h"
#include "words.h"

namespace jarvis
{
   extern char *jarvis_data;
   extern thread_manager_t *thread_manager;
   
   void jarvis_init(int argc, char **argv);
};

#endif
