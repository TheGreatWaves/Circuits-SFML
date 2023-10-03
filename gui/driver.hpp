/**
 * This file holds the main driver for the GUI loop.
 */
#pragma once
#ifndef GUI_DRIVER
#define GUI_DRIVER

/**
 * SFML Dependencies.
 */
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

/**
 * Simulator related dependencies.
 */
#include "context.hpp"
#include "board_gui.hpp"

/**
 * The program class represents the GUI application.
 */
class Program
{
public:

  explicit Program();
  Program(const Program&)            = delete;  // Disable copy constructor.
  Program(Program&&)                 = delete;  // Disable move constructor.
  Program& operator=(const Program&) = delete;  // Disable copy assignment.
  Program& operator=(Program&&)      = delete;  // Disable move assignment.
  ~Program()                         = default; // Default destructor.
  
  /**
   * Main application loop lives here.
   */
  void run();

  /**
   * This function calls update on all components.
   */
  void update(const sf::Time& dt);

  void fixed_time_updated();

  /**
   * Update delta time.
   */
  void update_dt();

  /**
   * Update things like mouse position etc.
   */
  void real_time_update();

  /**
   * This function calls draw on all components.
   */
  void draw();

  void handle_events();

private:
  // The actual window we're rendering to. 
  sf::RenderWindow m_window;

  /**
   * Time related members.
   */

  // Used to make updates consistent.
  const sf::Time TIME_PER_FRAME;
  sf::Time       m_time_since_last_update = sf::Time::Zero;
  sf::Clock      m_clock;

  /**
   * Simulator related members
   */
  Context       m_ctx;  // Retrievable via Context::instance() as well.
  BoardGui      m_board;
};

inline Program::Program()
: m_window(sf::VideoMode(1000, 800), "Workspace")
, TIME_PER_FRAME(sf::seconds(1.f/144.f))
, m_board(static_cast<sf::Vector2f>(m_window.getSize()))
{
  // WARNING!
  // Yes, frown at me. I deserve it.
  Context::instance()->window = &m_window;
}

inline void Program::run()
{
  while (m_window.isOpen())
  {
    // Update delta time.
    update_dt();

    // Handle events.
    handle_events();

    // Real time update.
    real_time_update();

    // Update. (fixed)
    update(m_time_since_last_update);

    // Render everything.
    draw();
  }
}

inline void Program::update_dt()
{
  m_time_since_last_update = m_clock.restart();
}

inline void Program::real_time_update()
{
}

inline void Program::fixed_time_updated()
{
  while (m_time_since_last_update >= TIME_PER_FRAME)
  {
      m_time_since_last_update -= TIME_PER_FRAME;
      update(TIME_PER_FRAME);
  }
}

inline void Program::update(const sf::Time& dt)
{
  m_board.update(dt);
}

inline void Program::draw()
{
  m_window.clear();

  /**
   * Call component's draw() function below.
   */

  // Draw the board
  m_board.draw(m_window, sf::RenderStates::Default);

  m_window.display();
}

inline void Program::handle_events()
{
  sf::Event event{};
  while (m_window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
    {
      m_window.close();
    }

    m_board.handle_events(event);
  }
}


#endif /* GUI_DRIVER */