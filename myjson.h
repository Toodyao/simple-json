#ifndef MY_JSON_H__
#define MY_JSON_H__

#include <stddef.h> // size_t

typedef enum { JSON_NULL, JSON_FALSE, JSON_TRUE, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT } json_type;

typedef struct json_value json_value;
typedef struct json_member json_member;

struct json_value{
    // Use union to make sure that only one element exists(object, array, string, number).
    union {
        // member
        struct {
            json_member* m; // Stores object
            size_t size;    // Indicate size of the object
        } o;
        // array
        struct {
           json_value* e; // Stores array
           size_t size;   // Indicate size of the array
        } a;
        // string
        struct {
            char* s;
            size_t len;
        } s;
        // number
        double n;
    } u;
    json_type type;
};

struct json_member {
    char* k;      // Key string
    size_t klen;  // Size of key string
    json_value v; // Value of member
};

enum {
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

#define json_init(v) do { (v)->type = JSON_NULL; } while(0)
#define json_set_null(v) json_free(v)

int json_parse(json_value* v, const char* json);

void json_free(json_value* v);

json_type json_get_type(const json_value* v);

int json_get_boolean(const json_value* v);
void json_set_boolean(json_value* v, int b);

double json_get_number(const json_value* v);
void json_set_number(json_value* v, double n);

const char* json_get_string(const json_value* v);
size_t json_get_string_length(const json_value* v);
void json_set_string(json_value* v, const char* s, size_t len);

size_t json_get_array_size(const json_value* v);
json_value* json_get_array_element(const json_value* v, size_t index);

size_t json_get_object_size(const json_value* v);
const char* json_get_object_key(const json_value* v, size_t index);
size_t json_get_object_key_length(const json_value* v, size_t index);
json_value* json_get_object_value(const json_value* v, size_t index);

#endif /* MY_JSON_H__ */
