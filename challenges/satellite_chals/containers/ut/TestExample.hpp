#include "gtest/gtest.h"


template<typename T> class Test1 : public ::testing::Test
{
protected:
    int x;
};
template class Test1<uint8_t>;
template class Test1<uint16_t>;
typedef testing::Types<uint8_t, uint16_t>  t;
//typedef testing::Types<uint8_t, uint16_t,uint32_t, uint64_t, float, double>  circ_type_list;
TYPED_TEST_SUITE(Test1 , t);

TYPED_TEST(Test1, Eq)
{
    this->x=1;
    EXPECT_EQ(1,this->x);
}