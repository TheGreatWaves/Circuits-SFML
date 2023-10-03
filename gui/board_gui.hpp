#pragma once
#ifndef BOARD_GUI
#define BOARD_GUI

#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "text_box.hpp"
#include "pin_port_gui.hpp"
#include "wire_gui.hpp"
#include "belt_gui.hpp"
#include "component_gui.hpp"

class BoardGui 
{
public:
  BoardGui(const sf::Vector2f& size)
  : m_mode_text_box("Normal mode", false)
  , m_belt(size)
  {
    m_background.setSize(size);
    m_background.setFillColor(sf::Color(30, 30, 46));

    m_prototype.setFillColor(sf::Color(30, 30, 46));
    m_prototype.setOutlineColor(sf::Color(49, 50, 68));
    m_prototype.setOutlineThickness(2.f);
    m_prototype.setSize({size.x/1.1f, size.y/1.2f});
    m_prototype.setPosition((m_background.getSize() - m_prototype.getSize()) / 2.f);

    m_name_text_box.set_position({m_prototype.getPosition().x, 20.f});
    m_mode_text_box.set_position({m_prototype.getPosition().x + m_prototype.getSize().x - m_mode_text_box.get_width(), 20.f});

    auto prototype_position = m_prototype.getPosition();

    m_input_pin_port.anchor(m_prototype);
    m_input_pin_port.set_interactability(true);

    m_output_pin_port.anchor(m_prototype, false);
    m_output_pin_port.set_interactability(false);

    // TODO: REMOVE THIS.
    // Let's add nand for now.
    m_components.emplace_back("nand");

    m_components.back().set_position({400.f, 400.f});
  }

  void update(const sf::Time& dt)
  {
    if (m_name_text_box.was_edited())
    {
      Context::instance()->current_component_name = m_name_text_box.get_string();
    }

    m_belt.update(dt);

    for (auto& wire : m_wires)
    {
      wire.update(dt);
    }

    for (auto& component : m_components)
    {
      component.update(dt);
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
      component.draw(target, states);
    }

    for (auto& wire : m_wires)
    {
      wire.draw(target, states);
    }

    m_belt.draw(target, states);
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
  }

  void handle_edit_mode(const sf::Event& event)
  {
    m_input_pin_port.handle_events(event);
    m_output_pin_port.handle_events(event);
  }

  void handle_wiring_mode(const sf::Event& event)
  {
    if (event.type == sf::Event::MouseButtonPressed)
    {
      auto mouse_pos = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

      // Retrieve pin.
      // TODO: Make this better.
      // This is pretty horrible, but let's do it for now.
      // This is the kind of code that keeps me awake at night.
      auto pin = m_input_pin_port.get_pin(mouse_pos);
      if (pin == nullptr) pin = m_output_pin_port.get_pin(mouse_pos);
      for (auto& component : m_components)
      {
        if (pin == nullptr) pin = component.get_input_pin_port()->get_pin(mouse_pos);
        if (pin == nullptr) pin = component.get_output_pin_port()->get_pin(mouse_pos);
      }

      auto* active_wire = Context::instance()->active_wire;

      if (active_wire != nullptr)
      {
        if (pin != nullptr && pin != active_wire->get_src_pin())
        {
          active_wire->add_node(pin->get_position());
          active_wire->add_dest_pin(pin);
          Context::instance()->active_wire = nullptr;
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
        wire.add_node(pin->get_position());
        Context::instance()->active_wire = &wire;
      }
    }
  }

  void handle_events(const sf::Event& event)
	{
    auto* context = Context::instance();

    m_name_text_box.handle_events(event);
    m_belt.handle_events(event);

    for (auto& component : m_components)
    {
      component.handle_events(event);
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
    m_input_pin_port.clear_port();
    m_output_pin_port.clear_port();
  }

private:
  sf::RectangleShape m_background;
  sf::RectangleShape m_prototype;
  TextBoxGui         m_name_text_box;
  TextBoxGui         m_mode_text_box;

  PinPortGui m_input_pin_port;
  PinPortGui m_output_pin_port;

  BeltGui m_belt;

  std::vector<WireGui> m_wires;
  std::vector<ComponentGui> m_components;
};

#endif /* BOARD_GUI */
