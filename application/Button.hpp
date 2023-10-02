#pragma once

// Core
#include <iostream> // For debugging
#include <string.h>
#include <memory>
#include <functional>

// SFML
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

// Other
#include "Mouse.hpp"
#include "Object.hpp"
#include "ResourceID.hpp"


// Forward declarations.
class Keyboard;

/////////////////////
// Config Structures
/////////////////////
struct ButtonConfig
{
	// transformations
	float x;
	float y;
	float width;
	float height;

	// color
	sf::Color idle_color;
	sf::Color hover_color;
	sf::Color active_color;
};

struct HandleConfig
{
	// transformations
	float x;
	float y;
	float width;
	float height;

	// color
	sf::Color idle_color;
	sf::Color hover_color;
	sf::Color active_color;
};

struct TextConfig
{

	float text;
	FontID font_id;

	// alignment
	bool align_horizontal;
	bool align_vertical;
	bool handle_comp_horizontal; // compensates for the handle space
	bool handle_comp_vertical; // compensates for the handle space

};

// Btn 
enum class State
{
	IDLE = 0,
	HOVER,
	ACTIVE,
};


enum class alignment
{
	
};

class Button : public Object
{
public: // Public methods.

	// Ctor through config
	Button(ButtonConfig& btn_conf,
		sf::Font& font, std::string label, bool dragAble) noexcept;

	// return button state
	const void outBoundClick(Mouse& mouse) noexcept;

	// return if a given shape contains mouse
	[[nodiscard]] const bool shapeContainsPoint(const sf::Shape& shape, const sf::Vector2f& p) const noexcept; // ask prompt on changing this

	[[nodiscard]] bool functionExists();

	[[nodiscard]] bool clicked(Mouse& mouse);

	// translates button and its components
	void moveButton(const sf::Vector2f& translation_v) noexcept;

	void updatePos() noexcept;

	void updateLabelAlignment();

	// returns button state
	const bool state() const noexcept;

	// update button
	void updateState(Mouse& mouse) noexcept;

	void update(Mouse& mouse) noexcept;

	// set function
	void setFunction(std::function<void()> function);

	// invoke function
	void invoke();

	void setLabel(const std::string& str);

private: // Private methods.
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	[[nodiscard]] virtual const bool shapeContainsPoint(const Mouse& mouse) const noexcept override;
private: // Private attributes

	// Button
	State mBtnState = State::IDLE;
	sf::RectangleShape mBtnShape;

	sf::Vector2f mBtnPos; // x, y
	sf::Vector2f mBtnSize; // width, height

	sf::Color mBtnIdleColor;
	sf::Color mBtnHoverColor;
	sf::Color mBtnActiveColor;

	// Handle
	std::unique_ptr<Button> handle;
	
	// Label
	sf::Font* mFont;
	sf::Text mLabel;

	sf::Vector2f mLabelPos;
	sf::Vector2f mLabelOffsetPos;

	bool mLabelAlignHorizontal = true;
	bool mLabelAlignVertical = true;
	bool mHandleCompHorizontal = false; // compensates for the handle space
	bool mHandleCompVertical = false; // compensates for the handle space


	// Error Color
	sf::Color mErrorColor;

	// Store result of outbounds check
	bool isHeld = false;
	bool outOfBounds = false;
	bool boundsUpdate = false;

	// Variables for dragging
	bool mIsDrag = false;
	bool canDrag;

	// temp
	sf::RectangleShape debug_point;

	std::function<void()> func;
};

//Ctor
inline Button::Button(ButtonConfig& btn_conf,
	sf::Font& font, std::string label, bool dragAble) noexcept
	: mBtnPos{btn_conf.x, btn_conf.y}
	, mBtnSize{ btn_conf.width, btn_conf.height }
	, mFont(&font)
	, mBtnIdleColor{ btn_conf.idle_color }
	, mBtnHoverColor{ btn_conf.hover_color }
	, mBtnActiveColor{ btn_conf.active_color }
	, canDrag(dragAble)
	, boundsUpdate(true)
{
	this->mBtnShape.setPosition(mBtnPos);
	this->mBtnShape.setSize(mBtnSize);

	// label
	this->mLabel.setFont(*this->mFont);
	this->mLabel.setString(label);
	this->mLabel.setFillColor(sf::Color::White);
	this->mLabel.setCharacterSize(30);


	// btn fill color
	this->mBtnShape.setFillColor(this->mBtnIdleColor);

	// error color
	this->mErrorColor = sf::Color::Magenta;


	if (canDrag)
	{
		ButtonConfig handleCFG{ 0.f, 0.f, 500.f, 100.f, sf::Color(100, 100, 100), sf::Color(50, 50, 50), sf::Color(10, 10, 10)};
		this->handle = std::make_unique<Button>(handleCFG, *this->mFont, "HANDLE", false);
	}


	// error color
	this->mErrorColor = sf::Color::Magenta;

	//temp
	this->debug_point.setFillColor(mErrorColor);
	this->debug_point.setSize(sf::Vector2f(5, 5));
}

// get button state
inline const bool Button::state() const noexcept
{
	return this->mBtnState == State::ACTIVE;
}

// Checks out of bound clicks
inline const void Button::outBoundClick(Mouse& mouse) noexcept
{
	isHeld = mouse.state();
	
	if (boundsUpdate && isHeld)
	{
		outOfBounds = !shapeContainsPoint(mouse);
		boundsUpdate = false;
	}

	if (!isHeld && !boundsUpdate)
	{
		boundsUpdate = true;
	}
	
}

// checks if point is in the bounds of a shape
inline const bool Button::shapeContainsPoint(const sf::Shape& shape, const sf::Vector2f& p) const noexcept
{
	return shape.getGlobalBounds().contains(p);
}

// move button using a vector
inline void Button::moveButton(const sf::Vector2f& translation_v) noexcept
{
	mBtnPos += translation_v;

	if (canDrag)
		this->handle->moveButton(translation_v);
}


inline void Button::updateLabelAlignment()
{

	if (mLabelAlignHorizontal)
	{
		mLabelPos.x =
			(mBtnPos.x + mBtnSize.x / 2.0f) - mLabel.getGlobalBounds().width / 2.0f;
	}
	else
		mLabelPos.x = mBtnPos.x;
	
	if (mLabelAlignVertical)
	{
		mLabelPos.y =
			(mBtnPos.y + mBtnSize.y / 2.0f - mLabel.getGlobalBounds().height / 2.0f);
	}
	else
		mLabelPos.x = mBtnPos.x;

	this->mLabel.setPosition(mLabelPos);

	this->debug_point.setPosition(mLabelPos);
}

inline void Button::updatePos() noexcept
{
	this->mBtnShape.setPosition(mBtnPos);

	mLabelOffsetPos = sf::Vector2f(0, 40);
	this->mLabel.setPosition(
		this->mBtnShape.getPosition().x + this->mBtnShape.getGlobalBounds().width / 2.f - this->mLabel.getGlobalBounds().width / 2.f + mLabelOffsetPos.x,
		this->mBtnShape.getPosition().y + this->mBtnShape.getGlobalBounds().height / 2.f - this->mLabel.getGlobalBounds().width / 2.f + mLabelOffsetPos.y);

	if (canDrag)
		this->handle->updatePos();
	mLabelOffsetPos = sf::Vector2f(0, 0);
	updateLabelAlignment();

}

inline bool Button::functionExists()
{
	return func != nullptr;
}

inline void Button::updateState(Mouse& mouse) noexcept
{
	// Flags if click occured out of bounds
	outBoundClick(mouse);

	////////////
	// BUTTON
	////////////

	std::cout << std::boolalpha;

	// Idle
	this->mBtnState = State::IDLE;
	if (functionExists() 
		&& !outOfBounds 
		&& handle->mBtnState == State::IDLE
		&& clicked(mouse))
	{
		this->invoke();
	}

	if (shapeContainsPoint(mouse)) // ask prompt how to reference mouse instead of copying
	{
		// hover
		this->mBtnState = State::HOVER;
		if (canDrag)
		{
			if (!handle->shapeContainsPoint(mouse))
			{
				this->mBtnState = State::HOVER;

				if (mouse.state() && !outOfBounds)
				{
					// active
					this->mBtnState = State::ACTIVE;
				}
			}
			else
			{
				this->mBtnState = State::IDLE;
			}
		}
		else
		{
			if (mouse.state() && !outOfBounds)
			{
				// active
				this->mBtnState = State::ACTIVE;
			}
		}
	}

	switch (this->mBtnState) // switch colors according to state
	{
	case State::IDLE:
		this->mBtnShape.setFillColor(this->mBtnIdleColor);
		break;

	case State::HOVER:
		this->mBtnShape.setFillColor(this->mBtnHoverColor);
		break;

	case State::ACTIVE:
		this->mBtnShape.setFillColor(this->mBtnActiveColor);
		break;

	default:
		this->mBtnShape.setFillColor(this->mErrorColor);
		break;

	}

	///////////
	// HANDLE
	///////////

	if (canDrag)
	{
		this->handle->updateState(mouse);

		if (mouse.state() && this->handle->shapeContainsPoint(mouse) && !outOfBounds)
		{
			moveButton(mouse.getMouseMotionVector());
		}
		
	}
		
}

inline bool Button::clicked(Mouse& mouse)
{
	return shapeContainsPoint(mouse) && mouse.isPressed();
}


inline void Button::setLabel(const std::string& str)
{
	this->mLabel.setString(str);
}

inline void Button::update(Mouse& mouse) noexcept
{
	updateState(mouse);

	updatePos();
	/*std::cout << "Motion Vector: " << mouse.getMouseMotionVector().x << ", " << mouse.getMouseMotionVector().y << '\n';
	std::cout << "Btn Pos: " << mBtnPos.x << ", " << mBtnPos.y << '\n';*/
}

inline void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// Order matters

	// Draw btn
	target.draw(mBtnShape);

	// Draw handle
	if (canDrag)
	{
		target.draw(*handle);
	}
	

	// Draw mLabel
	target.draw(mLabel);

	// Draw debug_pivot
	target.draw(debug_point);
}

inline const bool Button::shapeContainsPoint(const Mouse& mouse) const noexcept
{
	return mBtnShape.getGlobalBounds().contains(mouse.position());
}

inline void Button::invoke()
{
	this->func();
}

inline void Button::setFunction(std::function<void()> function)
{
	this->func = function;
}
