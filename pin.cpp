#include "pin.hpp"
#include "wire.hpp"
#include "gate.hpp"

void Pin::simulate(std::vector<bool>* visited, 
    std::map<std::size_t, std::unique_ptr<Gate>>* components)
{
  /** 
   * Simulate connection.
   */
  for (auto& connection : connections)
  {
    connection->simulate(visited, components);
  }
}
