#include "gtest/gtest.h"

#include <iostream>
#include "myjson.h"
using namespace std;

enum{j_null = 0};

TEST(JsonTypeTest, NullTest) {
    MyJson j;
    j._type = JSON_FALSE;
    EXPECT_EQ((int)JSON_FALSE, j.type());

    j._type = JSON_NULL;
    EXPECT_EQ((int)JSON_NULL, j.type());
}

GTEST_API_ int main (int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

