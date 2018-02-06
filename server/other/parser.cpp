#include "parser.h"

void split(const std::string &s,std::vector <std::string>  & v){
    std::string temp(s);
    //temp.pop_back();
    const char* delim=" ";
    char * dup = strdup(temp.c_str());
    char *token;
    while ((token = strtok_r(dup, delim,&dup))){
        v.push_back(std::string(token));
        //token = strtok_r(dup, delim,&dup);
    }
    //free(dup);
}
