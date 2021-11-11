#ifndef __CONCRETE_PRODUCT_A1_HPP
#define __CONCRETE_PRODUCT_A1_HPP

#include "AbstractProductA.hpp"

/**
 * Concrete Products are created by corresponding Concrete Factories.
 */
class ConcreteProductA1 : public AbstractProductA {
 public:
  std::string UsefulFunctionA() const override;
};

#endif  //  __CONCRETE_PRODUCT_A1_HPP