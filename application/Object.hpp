#pragma once

// SFML
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Shape.hpp>

// Others
#include "Mouse.hpp"

class Object : public sf::Drawable
{
protected:
	// return if a given shape contains mouse
	[[nodiscard]] virtual const bool shapeContainsPoint(const Mouse& mouse) const noexcept = 0; 
};
