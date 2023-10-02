#pragma once

// Core
#include <array>

// SFML
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>

// Others
#include "Debug.hpp"

class Input : public Debugable
{
	virtual void handleEvent(const sf::Event& event) noexcept = 0;
};
