#include "myjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod()*/
#include <string.h>

#ifndef JSON_PARSE_STACK_INIT_SIZE
#define JSON_PARSE_STACK_INIT_SIZE 256
#endif

// Check if the first character of c->json equals to ch
// and move the pointer to the next position.
#define EXPECT(c, ch)       do { assert((c)->json[0] == (ch)); (c)->json++; } while(0)

// Push the value to the stack by using the returned pointer of json_context_push()
#define PUTC(c, ch)         do { *(char*)json_context_push(c, sizeof(char)) = (ch); } while(0)

typedef struct {
    const char* json;
    char* stack;
    size_t size, top;
} json_context;

// Push context to dynamic stack, returns a void pointer.
// Modify the stack data by changing the data void pointer pointed.
static void* json_context_push(json_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = JSON_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* json_context_pop(json_context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

static int json_parse_string_raw(json_context* c, char** str, size_t* len) {
    size_t head = c->top;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    while (1) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                *len = c->top - head;
                *str = json_context_pop(c, *len);
                c->json = p;
                return JSON_PARSE_OK;
            case '\0':
                c->top = head;
                return JSON_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(c, ch);
        }
    }
}

static int json_parse_string(json_context* c, json_value* v) {
    int ret;
    char* s;
    size_t len;
    if ((ret = json_parse_string_raw(c, &s, &len)) == JSON_PARSE_OK)
        json_set_string(v, s, len);
    return ret;
}

static void json_parse_whitespace(json_context* c) {
    const char* p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int json_parse_null(json_context* c, json_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return JSON_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = JSON_NULL;
    return JSON_PARSE_OK;
}

static int json_parse_true(json_context* c, json_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return JSON_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = JSON_TRUE;
    return JSON_PARSE_OK;
}

static int json_parse_false(json_context* c, json_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return JSON_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = JSON_FALSE;
    return JSON_PARSE_OK;
}

static int json_parse_literal(json_context* c, json_value* v, const char* literal, json_type type) {
    EXPECT(c, literal[0]);

    size_t i;
    for (i = 0; literal[i+1] != '\0'; i++)
        if (c->json[i] != literal[i+1])
            return JSON_PARSE_INVALID_VALUE;

    c->json += i;
    v->type = type;
    return JSON_PARSE_OK;
}

static int json_parse_number(json_context* c, json_value* v) {
    char* end;
    // TODO: add number validation
    v->u.n = strtod(c->json, &end); // Convert string to double
    if (c->json == end)
        return JSON_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = JSON_NUMBER;
    return JSON_PARSE_OK;
}

static int json_parse_value(json_context* c, json_value* v); // Forward declare
static int json_parse_array(json_context* c, json_value* v) {
    size_t size = 0;
    int ret;
    EXPECT(c, '[');
    json_parse_whitespace(c);
    if (*c->json == ']') {
        c->json++;
        v->type = JSON_ARRAY;
        v->u.a.size = 0;
        v->u.a.e = NULL;
        return JSON_PARSE_OK;
    }
    // Process recursively
    while(1) {
        json_value e;
        json_init(&e);
        if ((ret = json_parse_value(c, &e)) != JSON_PARSE_OK)
            break;
        // Push(or copy) element e into stack
        memcpy(json_context_push(c, sizeof(json_value)), &e, sizeof(json_value));
        size++;
        json_parse_whitespace(c);
        if (*c->json == ',') {
            c->json++;
            json_parse_whitespace(c);
        }
        else if (*c->json == ']') {
            c->json++;
            v->type = JSON_ARRAY;
            v->u.a.size = size;
            size *= sizeof(json_value);
            // Copy full buffer into json_value
            memcpy(v->u.a.e = (json_value*)malloc(size), json_context_pop(c, size), size);
            return JSON_PARSE_OK;
        }
        else {
            ret = JSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    // Free allocated memory when parsing failed
    for (size_t i = 0; i < size; i++)
        json_free((json_value*)json_context_pop(c, sizeof(json_value)));

    return ret;
}

static int json_parse_object(json_context* c, json_value* v) {
    size_t i, size;
    json_member m;
    int ret;
    EXPECT(c, '{');
    json_parse_whitespace(c);
    if (*c->json == '}') {
        c->json++;
        v->type = JSON_OBJECT;
        v->u.o.m = 0;
        v->u.o.size = 0;
        return JSON_PARSE_OK;
    }
    m.k = NULL;
    size = 0;
    while(1) {
        char* str;
        json_init(&m.v);

        // parse key
        if (*c->json != '"') {
            ret = JSON_PARSE_MISS_KEY;
            break;
        }
        if ((ret = json_parse_string_raw(c, &str, &m.klen)) != JSON_PARSE_OK)
            break;
        memcpy(m.k = (char*)malloc(m.klen + 1), str, m.klen);
        m.k[m.klen] = '\0';

        // parse ws colon ws
        json_parse_whitespace(c);
        if (*c->json != ':') {
            ret = JSON_PARSE_MISS_COLON;
            break;
        }
        c->json++;
        json_parse_whitespace(c);

        // parse value
        if ((ret = json_parse_value(c, &m.v)) != JSON_PARSE_OK)
            break;
        memcpy(json_context_push(c, sizeof(json_member)), &m, sizeof(json_member));
        size++;
        m.k = NULL; // ownership is transferred to member on stack

        // parse comma or right-curly-brace
        json_parse_whitespace(c);
        if (*c->json == ',') {
            c->json++;
            json_parse_whitespace(c);
        }
        else if (*c->json == '}') {
            size_t s = sizeof(json_member) * size;
            c->json++;
            v->type = JSON_OBJECT;
            v->u.o.size = size;
            memcpy(v->u.o.m = (json_member*)malloc(s), json_context_pop(c, s), s);
            return JSON_PARSE_OK;
        }
        else {
            ret = JSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    // Pop and free members on the stack
    free(m.k);
    for (i = 0; i < size; i++) {
        json_member* m = (json_member*)json_context_pop(c, sizeof(json_member));
        free(m->k);
        json_free(&m->v);
    }
    v->type = JSON_NULL;
    return ret;
}

static int json_parse_value(json_context* c, json_value* v) {
    switch (*c->json) { // Equals to c->json[0]
        case 'n':  return json_parse_literal(c, v, "null", JSON_NULL);
        case 't':  return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f':  return json_parse_literal(c, v, "false", JSON_FALSE);
        case '\"': return json_parse_string(c, v);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
        case '[':  return json_parse_array(c, v);
        case '{':  return json_parse_object(c, v);
        default:   return json_parse_number(c, v);
    }
}

int json_parse(json_value* v, const char* json) {
    assert(v != NULL);

    int ret;
    json_context c;
    c.json = json;
    c.stack = NULL;
    c.size = c.top = 0;
    json_init(v);

    json_parse_whitespace(&c);
    if ((ret = json_parse_value(&c, v)) == JSON_PARSE_OK) {
        json_parse_whitespace(&c);
        // Has extra chars at the end of the value, fail it
        if (*(c.json) != '\0') {
            v->type = JSON_NULL;
            ret = JSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    free(c.stack);
    return ret;
}

void json_free(json_value* v) {
    // TODO: fix memory leak
    assert(v != NULL);
    // Free allocated memory only when v is string type
    switch (v->type) {
        case JSON_STRING:
            free(v->u.s.s);
            break;
        case JSON_ARRAY:
            for (size_t i = 0; i < v->u.a.size; i++)
                json_free(&v->u.a.e[i]);
            free(v->u.a.e);
            break;
        case JSON_OBJECT:
            for (size_t i = 0; i < v->u.o.size; i++) {
                free(v->u.o.m[i].k);
                json_free(&v->u.o.m[i].v);
            }
            free(v->u.o.m);
            break;
        default: break;
    }
    v->type = JSON_NULL;
}

// Getter Begin
json_type json_get_type(const json_value* v) {
    assert(v != NULL);
    return v->type;
}

double json_get_number(const json_value* v) {
    assert(v != NULL && v->type == JSON_NUMBER);
    return v->u.n;
}

int json_get_boolean(const json_value* v) {
    assert(v != NULL && (v->type == JSON_TRUE || v->type == JSON_FALSE));
    return v->type == JSON_TRUE;
}

size_t json_get_string_length(const json_value* v) {
    assert(v != NULL && v->type == JSON_STRING);
    return v->u.s.len;
}

const char* json_get_string(const json_value* v) {
    assert(v != NULL && v->type == JSON_STRING);
    return v->u.s.s;
}

size_t json_get_array_size(const json_value* v) {
    assert(v != NULL && v->type == JSON_ARRAY);
    return v->u.a.size;
}

json_value* json_get_array_element(const json_value* v, size_t index) {
    assert(v != NULL && v->type == JSON_ARRAY);
    assert(index < v->u.a.size);
    return &v->u.a.e[index];
}

size_t json_get_object_size(const json_value* v) {
    assert(v != NULL && v->type == JSON_OBJECT);
    return v->u.o.size;
}

const char* json_get_object_key(const json_value* v, size_t index) {
    assert(v != NULL && v->type == JSON_OBJECT);
    assert(index < v->u.o.size);
    return v->u.o.m[index].k;
}

size_t json_get_object_key_length(const json_value* v, size_t index) {
    assert(v != NULL && v->type == JSON_OBJECT);
    assert(index < v->u.o.size);
    return v->u.o.m[index].klen;
}

json_value* json_get_object_value(const json_value* v, size_t index) {
    assert(v != NULL && v->type == JSON_OBJECT);
    assert(index < v->u.o.size);
    return &v->u.o.m[index].v;
}
// Getter End

// Setter Begin
// Every setter begin with json_free()
// to make sure the context is clean
// json_free() has assert()
void json_set_boolean(json_value* v, int b) {
    json_free(v);
    v->type = b ? JSON_TRUE : JSON_FALSE;
}

void json_set_number(json_value* v, double n) {
    json_free(v);
    v->u.n = n;
    v->type = JSON_NUMBER;
}

void json_set_string(json_value* v, const char* s, size_t len) {
    assert(v != NULL && (s != NULL || len == 0));
    json_free(v);
    v->u.s.s = (char*)malloc(len + 1);
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = JSON_STRING;
}
// Setter End