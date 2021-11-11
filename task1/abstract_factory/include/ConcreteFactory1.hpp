#ifndef __CONCRETE_FACTORY_1_HPP
#define __CONCRETE_FACTORY_1_HPP

#include "AbstractFactory.hpp"
#include "AbstractProductA.hpp"
#include "AbstractProductB.hpp"

/**
 * Concrete Factories produce a family of products that belong to a single
 * variant. The factory guarantees that resulting products are compatible. Note
 * that signatures of the Concrete Factory's methods return an abstract product,
 * while inside the method a concrete product is instantiated.
 */
class ConcreteFactory1 : public AbstractFactory {
 public:
  AbstractProductA *CreateProductA() const override;
 
  AbstractProductB *CreateProductB() const override;
};

#endif  //  __CONCRETE_FACTORY_1_HPP