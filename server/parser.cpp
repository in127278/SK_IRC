#include "parser.h"

void split(const std::string &s,std::vector <std::string>  & v){
    const char* delim=" ";
    char * dup = strdup(s.c_str());
    char * token = strtok(dup, delim);
    while(token != NULL){
        v.push_back(std::string(token));
        token = strtok(NULL, delim);
    }
    free(dup);
}
