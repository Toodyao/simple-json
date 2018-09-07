#ifndef SIMPLE_JSON_MYJSON_H
#define SIMPLE_JSON_MYJSON_H

#include "myjson_base.h"
#include <cstddef> // size_t
#include <string>  // string
using std::string

class MyJson : public JsonValue {
public:
    // Need to be private:
    string json_string;

    void parse_whitespace();
    int parse_value();


    // Real public:
    int parse();
    json_type type();

//    int get_boolean();
//    double get_number();
//    const char* get_string();
//    size_t get_string_length();
//    size_t get_array_size();
//    json_value* get_array_element();
//    size_t get_object_size();
//    const char* get_object_key();
//    size_t get_object_key_length();
//    json_value* json_get_object_value();

    json_type test() {
        return _type;
    }

};


#endif //SIMPLE_JSON_MYJSON_H
