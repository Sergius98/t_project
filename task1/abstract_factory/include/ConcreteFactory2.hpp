#ifndef __CONCRETE_FACTORY_2_HPP
#define __CONCRETE_FACTORY_2_HPP

#include "AbstractFactory.hpp"
#include "AbstractProductA.hpp"
#include "AbstractProductB.hpp"


/**
 * Each Concrete Factory has a corresponding product variant.
 */
class ConcreteFactory2 : public AbstractFactory {
 public:
  AbstractProductA *CreateProductA() const override;
  AbstractProductB *CreateProductB() const override;
};

#endif  //  __CONCRETE_FACTORY_2_HPP