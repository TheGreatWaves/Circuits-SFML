#pragma once
#ifndef COMPONENT_RECIPE
#define COMPONENT_RECIPE

#include <vector>
#include <string_view>
#include <unordered_set>
#include <string>
#include <fstream>

#include "common.hpp"
#include "wire_info.hpp"

class Gate;

class ComponentRecipe
{
public:
  void add_dependency(const std::string& component) noexcept
  {
    m_dependencies.emplace(component);
  }

  void set_component_name(const std::string& name) noexcept
  {
    m_component_name = name;
  }

  void set_input_count(std::size_t ipc) noexcept
  {
    m_input_count = ipc;
  }

  void set_output_count(std::size_t opc) noexcept
  {
    m_output_count = opc;
  }

  void add_sub_component(const std::string& name)
  {
    add_dependency(name);
    m_components.emplace_back(name);
  }

  void set_serializable()
  {
    m_serializable = true;
  }

  void set_not_serializable()
  {
    m_serializable = false;
  }

  void add_wire_info(const WireInfo& wire_info)
  {
    m_wire_configuration.push_back(wire_info);
  }

  void set_wire_configuration(const WireConstructionInfo& wire_construction_info)
  {
    m_wire_configuration = wire_construction_info;
  }

  static ComponentRecipe construct_recipe(const Gate* gate);

  void create_recipe_file()
  {
    std::ofstream file { 
      DEFAULT_GATE_DIRECTORY + 
      std::string("/") +  
      DEFAULT_RECIPE_SAVE_DIRECTORY + 
      std::string("/") +  
      m_component_name + 
      GATE_EXTENSION 
    };

    // Require all dependencies.
    for (const auto& dependency : m_dependencies)
    {
      file << "need " << dependency << '\n';
    }

    // Declare the component.
    file << "create " << m_component_name << '\n';

    // Construct all the main input pins.
    for (std::size_t i = 0; i < m_input_count; i++)
    {
      file << "i +\n";
    }

    // Construct all the main output pins.
    for (std::size_t o = 0; o < m_output_count; o++)
    {
      file << "o +\n";
    }

    // Add all sub components.
    for (const auto& component : m_components)
    {
      file << "add " << component << '\n';
    }

    // Write all wiring information.
    for (const auto&[src, dest] : m_wire_configuration)
    {
      file << "wire " << src << ' ' << dest << '\n';
    }

    // Serialize if available.
    if (m_serializable)
    {
      file << "e\n";
    }

    // Reset context.
    file << "x\n";

    file.close();
  }

private:
  std::unordered_set<std::string> m_dependencies{};
  std::string                     m_component_name{};
  std::size_t                     m_input_count{};
  std::size_t                     m_output_count{};
  std::vector<std::string>        m_components{};
  std::vector<WireInfo>           m_wire_configuration{};
  bool                            m_serializable{ false };
};

#endif /* COMPONENT_RECIPE */
