#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "server.h"
#include <pthread.h>

class threads_manager {
public:
  threads_manager(int);
  ~threads_manager();
  int size;
  pthread_t *thread_list;
  int *running;
  int* cl_index;
  server* pointer;
  void empty(int);
};


#endif //CONNECTION_MANAGER_H
