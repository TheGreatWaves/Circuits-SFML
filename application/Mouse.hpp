#pragma once

// Others
#include "Input.hpp"

// Mouse wrapper class.
class Mouse : public Input
{
public: // Public methods.

	// Ctor.
	Mouse() noexcept;
 
	// Function to plug into the SFML event loop.
	// This function updates the state of the mouse.
	// It will manage everything the mouse needs.
	virtual void handleEvent(const sf::Event& event) noexcept override;

	// Updates the mouse position.
	void updatePosition(const sf::Window& window) noexcept;

	void updatePrevPosition(sf::Vector2f mousePos) noexcept;

	// Get mouse state.
	[[nodiscard]] bool state() const noexcept;

	[[nodiscard]] bool isPressed() noexcept
	{
		auto isPressed = this->mPressed;
		mPressed = false;
		return isPressed;
	}

	// Get mouse position.
	[[nodiscard]] const sf::Vector2f& position() const noexcept;

	// Get previous mouse position.
	[[nodiscard]] const sf::Vector2f& positionPrev() const noexcept;

	// Get mouse motion vector
	[[nodiscard]] sf::Vector2f getMouseMotionVector() const noexcept;

	// Output mouse info to stream.
	friend std::ostream& operator<<(std::ostream& s, const Mouse& mouse);

private: // Private attributes.

	sf::Vector2f		mPos;	//< Position of the mouse.
	bool				mHeld;
	bool				mPressed;

	sf::Vector2f		mPosPrev;  //< Previous mouse position

	// Inherited via Debugable
	virtual std::string info() const override;
	//< State of the mouse.
};


// Ctor.
inline Mouse::Mouse() noexcept
	: mPos(0.f, 0.f)
	, mHeld(false)
	, mPressed(false)
{}

// Read events and set flags accordingly.
inline void Mouse::handleEvent(const sf::Event & event) noexcept
{
	mPressed = false;
	switch (event.type)
	{
	// If mouse is pressed, set flag held to true.
	case sf::Event::MouseButtonPressed:
	{
		if (!mHeld && event.mouseButton.button == sf::Mouse::Left)
		{
			mPressed = false;
			mHeld = true;
		}
			
		break;
	}
	// If mouse is released, set flag held to false.
	case sf::Event::MouseButtonReleased:
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			mPressed = true;
			mHeld = false;
		}
		break;
	}
	// Event ignored.
	default:
		break;
	}
}

// Update mouse position relative to window.
inline void Mouse::updatePosition(const sf::Window& window) noexcept
{
	// Retrieve the new coordinates.
	auto [x, y] = sf::Mouse::getPosition(window);

	// Set current to point to the new coordinates.
	mPos.x = static_cast<float>(x);
	mPos.y = static_cast<float>(y);
}

inline void Mouse::updatePrevPosition(sf::Vector2f mousePos) noexcept
{
	mPosPrev = mousePos;
}

// Get mouse state.
inline bool Mouse::state() const noexcept
{
	return mHeld;
}

// Get mouse position.
inline const sf::Vector2f& Mouse::position() const noexcept
{
	return mPos;
}

inline const sf::Vector2f& Mouse::positionPrev() const noexcept
{
	return mPosPrev;
}

inline sf::Vector2f Mouse::getMouseMotionVector() const noexcept
{
	return mPos - mPosPrev;
}

inline std::string Mouse::info() const
{
	std::string str;
	str += "{Mouse Pos: ";
	str += std::to_string(mPos.x);
	str += ' ';
	str += std::to_string(mPos.y);
	str += " STATE: ";
	str += (mHeld ? "HELD" : "NOT HELD");
	return str;
}

// Out mouse info to stream.
inline std::ostream& operator<<(std::ostream& s, const Mouse& mouse)
{
	s << "{Mouse Pos: " << mouse.mPos.x << ' ' << mouse.mPos.y << " STATE: " << (mouse.mHeld ? "HELD" : "NOT HELD");
	return s;
}

// Redesign
// If clicked, capture current mouse position
// Get motion vector
// Vector 
