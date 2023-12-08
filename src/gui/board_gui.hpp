/** 
 * MIT License
 * 
 * Copyright (c) 2023 Ochawin A.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef BOARD_GUI
#define BOARD_GUI

#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "toolbox.hpp"
#include "text_box.hpp"
#include "wire_gui.hpp"
#include "component_gui.hpp"
#include "../component_recipe.hpp"
#include "../utils.hpp"

class BoardGui 
{
public:
  BoardGui(const sf::Vector2f& size)
  : m_bits_text_box("Bits: ", false)
  , m_connection_bits_text_box("1          ", true)
  , m_mode_text_box("Normal mode", false)
  , m_input_bits_text_box("Input Bits: ", false)
  , m_input_connection_bits_text_box("0", true)
  , m_toolbox(sf::Vector2f{TOOLBOX_WIDTH/2.f, 50.f})
  {
    // TODO: Yes this is bad.
    Context::instance()->board = this;
    m_sketch = std::make_unique<Gate>();    
    Context::instance()->sketch = m_sketch.get();

    m_background.setSize(size);
    m_background.setFillColor(sf::Color(30, 30, 46));

    m_prototype.setFillColor(sf::Color(30, 30, 46));
    m_prototype.setOutlineColor(sf::Color(49, 50, 68));
    m_prototype.setOutlineThickness(2.f);
    m_prototype.setSize({(size.x - TOOLBOX_WIDTH)/1.1f, size.y/1.2f});
    m_prototype.setPosition((sf::Vector2f{m_background.getSize().x + TOOLBOX_WIDTH, m_background.getSize().y} - m_prototype.getSize()) / 2.f);

    m_name_text_box.set_position({m_prototype.getPosition().x, 20.f});
    m_bits_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x  - m_mode_text_box.get_width() - 2 * m_connection_bits_text_box.get_width() - m_bits_text_box.get_width(), 20.f});
    m_connection_bits_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x  - m_mode_text_box.get_width() - 2 * m_connection_bits_text_box.get_width(), 20.f});
    m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});
    m_input_bits_text_box.set_position({m_prototype.getPosition().x, 740.f});
    m_input_connection_bits_text_box.set_position({m_prototype.getPosition().x + m_input_bits_text_box.get_width(), 740.f});

    auto prototype_position = m_prototype.getPosition();

    m_input_connection_port.anchor(m_prototype);
    m_input_connection_port.set_interactability(true);

    m_output_connection_port.anchor(m_prototype, false);
    m_output_connection_port.set_interactability(false);
  }

  void add_component(std::unique_ptr<ComponentGui> component)
  {
    m_components.push_back(std::move(component));
  }

  void update(const sf::Time& dt)
  {
    if (m_name_text_box.was_edited())
    {
      Context::instance()->current_component_name = m_name_text_box.get_string();
    }

    // m_belt.update(dt);
    m_toolbox.update(dt);

    for (auto& wire : m_wires)
    {
      wire.update(dt);
    }

    if (m_connection_bits_text_box.was_edited())
    {
      if (!m_connection_bits_text_box.get_string().empty())
      {
        Context::instance()->bus_bits = std::stoi(m_connection_bits_text_box.get_string());
      }
    }

    if (m_input_connection_bits_text_box.was_edited())
    {
      if (!m_input_connection_bits_text_box.get_string().empty())
      {
        Context::instance()->input_bus_bits = std::stoi(m_input_connection_bits_text_box.get_string());
      }
    }

    for (auto& component : m_components)
    {
      component->update(dt);
    }
   
  }

  void update_mode_text_box_string()
  {
    switch (Context::instance()->edit_mode)
      {
        break; case Mode::IDLE: 
        {
          m_mode_text_box.set_string("Normal mode");
        }
        break; case Mode::WIRING:
        {
          m_mode_text_box.set_string("Wiring mode");
        }
        default:
        {}
      }
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    update_mode_text_box_string();
    target.draw(m_background, states);
    target.draw(m_prototype, states);
    target.draw(m_name_text_box, states);
    target.draw(m_mode_text_box, states);
    target.draw(m_connection_bits_text_box, states);
    target.draw(m_input_connection_bits_text_box, states);
    target.draw(m_input_bits_text_box, states);
    target.draw(m_bits_text_box, states);

    m_input_connection_port.draw(target, states);
    m_output_connection_port.draw(target, states);

    for (auto& component : m_components)
    {
      component->draw(target, states);
    }

    for (auto& wire : m_wires)
    {
      wire.draw(target, states);
    }

    m_toolbox.draw(target, states);
  }

  void handle_keyboard(const sf::Event& event)
  {
    // Toggle between editting modes
    if (event.key.code == sf::Keyboard::E)
    {
      auto ctx = Context::instance();

      if (ctx->edit_mode == Mode::WIRING)
      {
        ctx->edit_mode = Mode::IDLE;
        m_mode_text_box.set_string("Normal mode");
        ctx->active_wire = nullptr;
      }
      else
      {
        ctx->edit_mode = Mode::WIRING;
        m_mode_text_box.set_string("Wiring mode");
      }

      m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});
    }
    else if (event.key.code == sf::Keyboard::C)
    {
      clear();
    }
  }

  void handle_edit_mode(const sf::Event& event)
  {
    m_input_connection_port.handle_events(event);
    m_output_connection_port.handle_events(event, false);
  }


// returns the PIN ID and the pin.
std::pair<std::size_t, ConnectionGui*> get_connection(const sf::Vector2f& pos)
{
  std::size_t input_pid_acc = 0;
  std::size_t output_pid_acc = 0;

  // Query main input pins.
  auto [connection_pid, connection] = m_input_connection_port.get_connection(pos);

  // Return if found from main input port.
  if (connection != nullptr) return {connection_pid, connection};
  input_pid_acc += m_input_connection_port.get_number_of_pins();

  // Query main output pins.
  std::tie(connection_pid, connection) = m_output_connection_port.get_connection(pos);

  // Return if found from main output port.
  if (connection != nullptr) return {connection_pid + INPUT_PIN_LIMIT, connection};
  output_pid_acc += m_output_connection_port.get_number_of_pins();

  // Query from components (this is where it gets messy).
  for (auto& component : m_components)
  {
    // Search the input port.
    std::tie(connection_pid, connection) = component->get_input_pin_port()->get_connection(pos);

    // Return if found from input port.
    if (connection != nullptr) return {input_pid_acc + connection_pid, connection};

    // Search the output port.
    std::tie(connection_pid, connection) = component->get_output_pin_port()->get_connection(pos);

    // Return if found from output port.
    if (connection != nullptr) return {output_pid_acc + connection_pid + INPUT_PIN_LIMIT, connection};

    // If we reached here it means that we haven't found them. We add to the i/o accumulators.
    input_pid_acc += component->get_input_pin_port()->get_number_of_pins();
    output_pid_acc += component->get_output_pin_port()->get_number_of_pins();
  }

  return {0, nullptr};
}

  void handle_wiring_mode(const sf::Event& event)
  {
    // TODO: Connect all the wires within the bus from src -> dest
    if (event.type == sf::Event::MouseButtonPressed)
    {
      auto mouse_pos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

      // Retrieve pin.
      auto [connection_pid, connection] = get_connection(mouse_pos);

      auto* active_wire = Context::instance()->active_wire;

      if (active_wire != nullptr)
      {
        if (connection != nullptr && connection != active_wire->get_src_pin().first)
        {
          // Go through all the pins in the connection and connect them up
          active_wire->add_node(connection->get_position() + connection->get_size()/2.f);
          auto active_wire_src = active_wire->get_src_pin();
          auto active_wire_connection_pid = active_wire->get_src_connection_idx();
          std::size_t pins = connection->get_number_of_pins();
          if (pins > active_wire->get_src_connection_size())
          {
            pins = active_wire->get_src_connection_size();
          }
          for (int index = 0; index < pins; index++)
          {
            m_wires.emplace_back();
            auto& new_wire = m_wires.back();
            new_wire.set_src_pin(active_wire_src.first, index);
            new_wire.set_src_connection_idx(active_wire_connection_pid);
            new_wire.set_src_index(active_wire_connection_pid + index);
            new_wire.set_dest_pin(connection, index);
            new_wire.set_src_connection_idx(connection_pid);
            new_wire.set_dest_index(connection_pid + index);
            new_wire.add_node(connection->get_position() + connection->get_size()/2.f);
            Context::instance()->sketch->wire_pins(new_wire.get_src_index(), new_wire.get_dest_index());
          }
          Context::instance()->active_wire = nullptr;
        }
        else
        {
          active_wire->handle_events(event);
        }
        return;
      }

      if (connection != nullptr)
      {
        m_wires.emplace_back();
        auto& wire = m_wires.back();

        wire.set_src_pin(connection, 0);
        wire.set_src_connection_idx(connection_pid);
        wire.set_src_index(connection_pid);
        wire.add_node(connection->get_position() + connection->get_size()/2.f);
        Context::instance()->active_wire = &wire;
      }
    }
  }

  void handle_events(const sf::Event& event)
	{
    auto* context = Context::instance();
    m_name_text_box.handle_events(event);
    m_connection_bits_text_box.handle_events(event);
    m_input_connection_bits_text_box.handle_events(event);
    m_toolbox.handle_events(event);

    for (auto& component : m_components)
    {
      component->handle_events(event);
    }

    if (event.type == sf::Event::KeyPressed && context->edit_mode != Mode::TEXT) handle_keyboard(event);

    switch (context->edit_mode)
    {
    break; case Mode::TEXT:
    break; case Mode::IDLE: handle_edit_mode(event);
    break; case Mode::WIRING: handle_wiring_mode(event);
    }
	}

  void clear()
  {
    m_wires.clear();
    m_components.clear();
    m_input_connection_port.clear_port();
    m_output_connection_port.clear_port();
    m_name_text_box.reset();

    m_sketch = nullptr;
    m_sketch = std::make_unique<Gate>();

    Context::instance()->sketch = m_sketch.get();
    Context::instance()->active_wire = nullptr;
  }

  void create_new_sketch(std::string_view new_component_name)
  {
    auto board = Board::instance();
    auto ctx = Context::instance();

    m_sketch = std::make_unique<Gate>();
    ctx->sketch = m_sketch.get();

    board->set_context(new_component_name);
  }


  void save_current_configuration(bool serialize = false)
  {
    auto ctx = Context::instance();
    auto board = Board::instance();
    auto current_component_name = ctx->current_component_name;

    auto name_not_empty = !current_component_name.empty();
    auto component_not_found = !Board::instance()->found(current_component_name);

    // We will only be able to save when the name isn't empty
    // and the component isn't already pre-existing.
    if (name_not_empty && component_not_found)
    {
      auto current = ctx->sketch;
      ctx->sketch->set_name(current_component_name);
      board->save_sketch(std::move(m_sketch));

      create_new_sketch(current_component_name);

      if (serialize)
      {
        current->serialize();
        current->print_truth_table();
      }

      ComponentRecipe recipe = ComponentRecipe::construct_recipe(current);
      recipe.set_wire_configuration(current->wire_construction_recipe);
      recipe.create_recipe_file();

      clear();
    }
  }

private:
  sf::RectangleShape m_background;
  sf::RectangleShape m_prototype;
  TextBoxGui         m_name_text_box;
  TextBoxGui         m_mode_text_box;
  TextBoxGui         m_connection_bits_text_box;
  TextBoxGui         m_bits_text_box;
  TextBoxGui         m_input_bits_text_box;
  TextBoxGui         m_input_connection_bits_text_box;

  ConnectionPortGui m_input_connection_port;
  ConnectionPortGui m_output_connection_port;

  ToolBox m_toolbox;

  std::vector<WireGui> m_wires;
  std::vector<std::unique_ptr<ComponentGui>> m_components;
  std::unique_ptr<Gate> m_sketch;
};

#endif /* BOARD_GUI */
