#ifndef __CONCRETE_PRODUCT_A2_HPP
#define __CONCRETE_PRODUCT_A2_HPP

#include "AbstractProductA.hpp"

/**
 * Concrete Products are created by corresponding Concrete Factories.
 */
class ConcreteProductA2 : public AbstractProductA {
  std::string UsefulFunctionA() const override;
};

#endif  //  __CONCRETE_PRODUCT_A2_HPP