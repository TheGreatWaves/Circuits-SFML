#include "pin.hpp"
#include "wire.hpp"
#include "digital_serializer.hpp"

void Pin::simulate()
{
  /** 
   * Simulate connection.
   */
  for (auto connection : connections)
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
