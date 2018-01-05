#include "connection_manager.h"
threads_manager::~threads_manager(){

}

threads_manager::threads_manager(int i){
  this->size=i;
  this->thread_list=new pthread_t[this->size];
  this->running=new int[this->size];
  this->cl_index=new int[this->size];
}
void threads_manager::empty(int index){
  this->running[index]=0;
}
