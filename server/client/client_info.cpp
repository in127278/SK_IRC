#include "client_info.h"

client_info::~client_info() {
}

client_info::client_info() {
  this->clientFd=-1;
  this->channel=-1;
  this->id=-1;
}
void client_info::reset(){
  this->clientFd=-1;
  this->channel=-1;
  this->id=-1;
}
