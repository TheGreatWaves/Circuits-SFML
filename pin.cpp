#include "pin.hpp"
#include "wire.hpp"
#include "digital_serializer.hpp"

void Pin::simulate()
{
  /** 
   * Simulate connection.
   */
  if (connection != nullptr)
  {
    connection->simulate();
  }

  /**
   * If the current pin is on a component, simulate it.
   */
  if (parent != nullptr)
  {
    parent->simulate();
  }
}
