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

#include "pin_gui.hpp"
#include "text_box.hpp"
#include "toolbox.hpp"
#include "wire_gui.hpp"


// #include "pin_port_gui.hpp"
#include "connection_port_gui.hpp"
#include "component_gui.hpp"
#include "../component_recipe.hpp"

class BoardGui 
{
public:
  BoardGui(const sf::Vector2f& size)
  : m_mode_text_box("Normal mode", false)
  , m_toolbox(sf::Vector2f{TOOLBOX_WIDTH/2.f, 50.f})
  , m_input_pin_port(PIN_RADIUS)
  , m_output_pin_port(PIN_RADIUS)
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
    m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});

    auto prototype_position = m_prototype.getPosition();

    m_input_pin_port.anchor(m_prototype);
    m_input_pin_port.set_interactability(true);

    m_output_pin_port.anchor(m_prototype, false);
    m_output_pin_port.set_interactability(false);
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


    for (auto& wire : m_wires)
    {
      wire.update(dt);
    }

    // std::cout << BLOCK << BLOCK << " [ UPDATING ALL COMPONENT ] " << BLOCK  << BLOCK << BLOCK << BLOCK << BLOCK << '\n';
    // m_input_pin_port.info();
    // Context::instance()->sketch->wire_info();
    // Context::instance()->sketch->subgates_brief();

    for (auto& component : m_components)
    {
      component->update(dt);
    }
   
  }

  void draw(sf::RenderTarget &target, sf::RenderStates states)
  {
    target.draw(m_background, states);
    target.draw(m_prototype, states);
    target.draw(m_name_text_box, states);
    target.draw(m_mode_text_box, states);

    m_input_pin_port.draw(target, states);
    m_output_pin_port.draw(target, states);

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

      // TODO: Make the wiring mode actually work
      switch (ctx->edit_mode)
      {
        break; case Mode::IDLE: 
        {
          ctx->edit_mode = Mode::WIRING;
          m_mode_text_box.set_string("Wiring mode");
          ctx->active_wire = nullptr;
        }
        break; case Mode::WIRING:
        {
          ctx->edit_mode = Mode::BUS;
          m_mode_text_box.set_string("Bus mode");
        }
        break; case Mode::BUS:
        {
          ctx->edit_mode = Mode::IDLE;
          m_mode_text_box.set_string("Normal mode");
        }
        default:
        {}
      }
      // if (ctx->edit_mode == Mode::WIRING)
      // {
      //   ctx->edit_mode = Mode::IDLE;
      //   m_mode_text_box.set_string("Normal mode");
      //   ctx->active_wire = nullptr;
      // }
      // else
      // {
      //   ctx->edit_mode = Mode::WIRING;
      //   m_mode_text_box.set_string("Wiring mode");
      // }

      m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});
    }
    /*else*/ if (event.key.code == sf::Keyboard::C)
    {
      clear();
    }
  }

  void handle_edit_mode(const sf::Event& event)
  {
    m_input_pin_port.handle_events(event, true);
    m_output_pin_port.handle_events(event, false);
  }


// returns the PIN ID and the pin.
std::pair<std::size_t, ConnectionGui*> get_pin(const sf::Vector2f& pos)
{
  std::size_t input_pid_acc = 0;
  std::size_t output_pid_acc = 0;

  // Query main input pins.
  auto [pid, pin] = m_input_pin_port.get_pin(pos);

  // Return if found from main input port.
  if (pin != nullptr) return {pid, pin};
  input_pid_acc += m_input_pin_port.size();

  // Query main output pins.
  std::tie(pid, pin) = m_output_pin_port.get_pin(pos);

  // Return if found from main output port.
  if (pin != nullptr) return {pid + INPUT_PIN_LIMIT, pin};
  output_pid_acc += m_output_pin_port.size();

  // Query from components (this is where it gets messy).
  for (auto& component : m_components)
  {
    // Search the input port.
    std::tie(pid, pin) = component->get_input_pin_port()->get_pin(pos);

    // Return if found from input port.
    if (pin != nullptr) return {input_pid_acc + pid, pin};

    // Search the output port.
    std::tie(pid, pin) = component->get_output_pin_port()->get_pin(pos);

    // Return if found from output port.
    if (pin != nullptr) return {output_pid_acc + pid + INPUT_PIN_LIMIT, pin};

    // If we reached here it means that we haven't found them. We add to the i/o accumulators.
    input_pid_acc += component->get_input_pin_port()->size();
    output_pid_acc += component->get_output_pin_port()->size();
  }

  return {0, nullptr};
}

  void handle_wiring_mode(const sf::Event& event)
  {
    if (event.type == sf::Event::MouseButtonPressed)
    {
      auto mouse_pos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

      // Retrieve pin.
      auto [pid, pin] = get_pin(mouse_pos);

      auto* active_wire = Context::instance()->active_wire;

      if (active_wire != nullptr)
      {
        if (pin != nullptr && pin != active_wire->get_src_pin())
        {
          active_wire->add_node(pin->get_position());
          active_wire->set_dest_pin(pin);
          active_wire->set_dest_index(pid);
          Context::instance()->active_wire = nullptr;

          Context::instance()->sketch->wire_pins(active_wire->get_src_index(), active_wire->get_dest_index());
        }
        else
        {
          active_wire->handle_events(event);
        }
        return;
      }

      if (pin != nullptr)
      {
        m_wires.emplace_back();
        auto& wire = m_wires.back();
        wire.set_src_pin(pin);
        wire.set_src_index(pid);
        wire.add_node(pin->get_position());
        Context::instance()->active_wire = &wire;
      }
    }
  }

  void handle_bus_mode(const sf::Event& event)
  {
    m_input_pin_port.handle_events(event, true, 1);
    m_output_pin_port.handle_events(event, false, 1);
  }

  void handle_events(const sf::Event& event)
	{
    auto* context = Context::instance();

    m_name_text_box.handle_events(event);
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
    break; case Mode::BUS: handle_bus_mode(event);
    }
	}

  void clear()
  {
    m_wires.clear();
    m_components.clear();
    m_input_pin_port.clear_port();
    m_output_pin_port.clear_port();
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

  // PinPortGui m_input_pin_port;
  // PinPortGui m_output_pin_port;

  ConnectionPortGui m_input_pin_port;
  ConnectionPortGui m_output_pin_port;



  ToolBox m_toolbox;

  std::vector<WireGui> m_wires;
  std::vector<std::unique_ptr<ComponentGui>> m_components;
  std::unique_ptr<Gate> m_sketch;
};

#endif /* BOARD_GUI */
