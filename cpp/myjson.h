#ifndef SIMPLE_JSON_MYJSON_H
#define SIMPLE_JSON_MYJSON_H

#include "myjson_base.h"
#include <cstddef> // size_t

class MyJson {
public:
    json_value j;

public:
    int parse();
    json_type type();

//    int get_boolean();
//    double get_number();
//    const char* get_string();
//    size_t get_string_length();
//    size_t get_array_size();
//    json_value* get_array_element();
//    size_t get_object_soze();
//    const char* get_object_key();
//    size_t get_object_key_length();
//    json_value* json_get_object_value();

    json_type test() {
        return j.type;
    }

};


#endif //SIMPLE_JSON_MYJSON_H
