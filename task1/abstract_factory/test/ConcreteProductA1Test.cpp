#include "ConcreteProductA1.hpp"

#include <gtest/gtest.h>


TEST(ConcreteProductA1Test, shouldSuccessfullyExecuteUsefulFunctionA)
{
    ConcreteProductA1 product;

    const std::string kExpectedResult{"The result of the product A1."};

    ASSERT_EQ(kExpectedResult, product.UsefulFunctionA());
}