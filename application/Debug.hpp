#pragma once

// Core
#include <sstream>
#include <cassert>
#include <iostream>

// SFML
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>


// Debuggable ensures that object information can be passed onto a stream.
class Debugable
{
public:
	virtual std::string info() const = 0;
};

// Debug class, used for displaying text onto the screen
class Debug : public sf::Drawable
{
public: // Public methods

	Debug(const sf::Font& font);

	// Register item.
	void reg(const Debugable& item);

	// Update debug text.
	void update();

private: // Private methods

	// Load font;
	void loadFont(const sf::Font& font);

	// Init text;
	void initText();

	// Inherited via Drawable
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private: // Private attributes
	std::stringstream				ss;
	const sf::Font*					mFont;
	sf::Text						mText;
	std::vector<const Debugable*>	trackedObjects;
};

inline Debug::Debug(const sf::Font& font)
{
	// Load font.
	loadFont(font);

	// Initialize the text.
	initText();
}

inline void Debug::reg(const Debugable& item)
{
	trackedObjects.push_back(&item);
}

inline void Debug::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mText, states);
}

inline void Debug::update()
{
	// Reset string stream.
	ss.str("");
	ss.clear();


	//// Fill string stream with info.
	for (const Debugable* item : trackedObjects)
	{
		ss << item->info() << '\n';
	}

	// Set text.
	mText.setString(ss.str());
}

inline void Debug::loadFont(const sf::Font& font)
{
	mFont = &font;
}

inline void Debug::initText()
{
	mText.setCharacterSize(20);
	mText.setFont(*mFont);
}

