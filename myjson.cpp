#include "myjson.h"
#include <cassert>  /* assert() */
#include <cstdlib>  /* NULL, strtod()*/

// Check if the first character of c->json equals to ch
// and move the pointer to the next position.
#define EXPECT(c, ch)       do { assert((c)->json[0] == (ch)); (c)->json++; } while(0)

typedef struct {
    const char* json;
} json_context;

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
    v->n = strtod(c->json, &end); // Convert string to double
    if (c->json == end)
        return JSON_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = JSON_NUMBER;
    return JSON_PARSE_OK;
}

static int json_parse_value(json_context* c, json_value* v) {
    switch (*c->json) {
        case 'n':  return json_parse_literal(c, v, "null", JSON_NULL);
        case 't':  return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f':  return json_parse_literal(c, v, "false", JSON_FALSE);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
        default:   return json_parse_number(c, v);
    }
}

int json_parse(json_value* v, const char* json) {
    int ret;
    json_context c;
    assert(v != NULL);
    c.json = json;
    v->type = JSON_NULL;

    json_parse_whitespace(&c);
    if ((ret = json_parse_value(&c, v)) == JSON_PARSE_OK) {
        json_parse_whitespace(&c);
        if (*(c.json) != '\0')
            ret = JSON_PARSE_ROOT_NOT_SINGULAR;
    }

    return ret;
}

json_type json_get_type(const json_value* v) {
    assert(v != NULL);
    return v->type;
}

double json_get_number(const json_value* v) {
    assert(v != NULL && v->type == JSON_NUMBER);
    return v->n;
}
