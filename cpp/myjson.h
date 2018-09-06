#ifndef SIMPLE_JSON_MYJSON_H
#define SIMPLE_JSON_MYJSON_H

#include "myjson_base.h"
#include <cstddef> // size_t

class MyJson {
private:
    json_value j;

public:
    int parse();
    json_type test() {
        return j.type;
    }

};


#endif //SIMPLE_JSON_MYJSON_H
