#ifndef SIMPLE_JSON_MYJSON_BASE_H
#define SIMPLE_JSON_MYJSON_BASE_H

#include <cstddef> // size_t

typedef enum { JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT } json_type;

typedef struct json_value json_value;
typedef struct json_member json_member;

struct json_value {
    // Use union to make sure that only one element exists(object, array, string, number).
    union {
        // member
        struct {
            json_member* m; // Stores object
            size_t size;    // Indicate size of the object
        } obj;
        // array
        struct {
            struct json_value* ele; // Stores array
            size_t size;   // Indicate size of the array
        } arr;
        // string
        struct {
            char* s;
            size_t len;
        } str;
        // number
        double num;
    };
    json_type type;
};

struct json_object {
    char* key;      // Key string
    size_t k_len;  // Size of key string
    json_value v; // Value of member
};

enum { // Json paring state
    JSON_PARSE_OK = 0,
    JSON_PARSE_EXPECT_VALUE,
    JSON_PARSE_INVALID_VALUE,
    JSON_PARSE_ROOT_NOT_SINGULAR,
    JSON_PARSE_MISS_QUOTATION_MARK,
    JSON_PARSE_INVALID_STRING_ESCAPE,
    JSON_PARSE_INVALID_STRING_CHAR,
    JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    JSON_PARSE_MISS_KEY,
    JSON_PARSE_MISS_COLON,
    JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET
};

#endif //SIMPLE_JSON_MYJSON_BASE_H
