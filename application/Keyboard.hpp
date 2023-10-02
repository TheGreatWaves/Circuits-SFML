#pragma once

// Others
#include "Input.hpp"

// Mouse wrapper class.
class Keyboard : public Input
{
public:

	// Handles the event 
	virtual void handleEvent(const sf::Event& event) noexcept override;

	// Returns whether the key is held or not.
	[[nodiscard]] constexpr bool isKeyHeld(sf::Keyboard::Key key) const noexcept;

	// Returns whether the key is pressed or not. (This function will release it)
	[[nodiscard]] constexpr bool isKeyPressed(sf::Keyboard::Key key) noexcept;

	template <typename... Args>
	[[nodiscard]] constexpr bool isKeyHeld(Args ... arg)
	{
		// Unary right fold 
		return (keys[arg] && ...);
	}

	// Output keyboard info to stream.
	friend std::ostream& operator<<(std::ostream& os, const Keyboard& keyboard);

private:
	// Array containing all the keys on the keyboard
	// and a boolean in their index indicating whether
	// they are held down or not.
	std::array<bool, sf::Keyboard::KeyCount> keys;

    size_t numberOfKeysHeld;

	// Array containg the keys as a char
	const std::array<std::string, sf::Keyboard::KeyCount> keyChars
	{
        "A",        ///< The A key
        "B",            ///< The B key
        "C",            ///< The C key
        "D",            ///< The D key
        "E",            ///< The E key
        "F",            ///< The F key
        "G",            ///< The G key
        "H",            ///< The H key
        "I",            ///< The I key
        "J",            ///< The J key
        "K",            ///< The K key
        "L",            ///< The L key
        "M",            ///< The M key
        "N",            ///< The N key
        "O",            ///< The O key
        "P",            ///< The P key
        "Q",            ///< The Q key
        "R",            ///< The R key
        "S",            ///< The S key
        "T",            ///< The T key
        "U",            ///< The U key
        "V",            ///< The V key
        "W",            ///< The W key
        "X",            ///< The X key
        "Y",            ///< The Y key
        "Z",            ///< The Z key
        "Num0",         ///< The 0 key
        "Num1",         ///< The 1 key
        "Num2",         ///< The 2 key
        "Num3",         ///< The 3 key
        "Num4",         ///< The 4 key
        "Num5",         ///< The 5 key
        "Num6",         ///< The 6 key
        "Num7",         ///< The 7 key
        "Num8",         ///< The 8 key
        "Num9",         ///< The 9 key
        "Escape",       ///< The Escape key
        "LControl",     ///< The left Control key
        "LShift",       ///< The left Shift key
        "LAlt",         ///< The left Alt key
        "LSystem",      ///< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
        "RControl",     ///< The right Control key
        "RShift",       ///< The right Shift key
        "RAlt",         ///< The right Alt key
        "RSystem",      ///< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
        "Menu",         ///< The Menu key
        "LBracket",     ///< The [ key
        "RBracket",     ///< The ] key
        "Semicolon",    ///< The ; key
        "Comma",        ///< The , key
        "Period",       ///< The . key
        "Quote",        ///< The ' key
        "Slash",        ///< The / key
        "Backslash",    ///< The \ key
        "Tilde",        ///< The ~ key
        "Equal",        ///< The = key
        "Hyphen",       ///< The - key (hyphen)
        "Space",        ///< The Space key
        "Enter",        ///< The Enter/Return keys
        "Backspace",    ///< The Backspace key
        "Tab",          ///< The Tabulation key
        "PageUp",       ///< The Page up key
        "PageDown",     ///< The Page down key
        "End",          ///< The End key
        "Home",         ///< The Home key
        "Insert",       ///< The Insert key
        "Delete",       ///< The Delete key
        "Add",          ///< The + key
        "Subtract",     ///< The - key (minus, usually from numpad)
        "Multiply",     ///< The * key
        "Divide",       ///< The / key
        "Left",         ///< Left arrow
        "Right",        ///< Right arrow
        "Up",           ///< Up arrow
        "Down",         ///< Down arrow
        "Numpad0",      ///< The numpad 0 key
        "Numpad1",      ///< The numpad 1 key
        "Numpad2",      ///< The numpad 2 key
        "Numpad3",      ///< The numpad 3 key
        "Numpad4",      ///< The numpad 4 key
        "Numpad5",      ///< The numpad 5 key
        "Numpad6",      ///< The numpad 6 key
        "Numpad7",      ///< The numpad 7 key
        "Numpad8",      ///< The numpad 8 key
        "Numpad9",      ///< The numpad 9 key
        "F1",           ///< The F1 key
        "F2",           ///< The F2 key
        "F3",           ///< The F3 key
        "F4",           ///< The F4 key
        "F5",           ///< The F5 key
        "F6",           ///< The F6 key
        "F7",           ///< The F7 key
        "F8",           ///< The F8 key
        "F9",           ///< The F9 key
        "F10",          ///< The F10 key
        "F11",          ///< The F11 key
        "F12",          ///< The F12 key
        "F13",          ///< The F13 key
        "F14",          ///< The F14 key
        "F15",          ///< The F15 key
        "Pause",        ///< The Pause key
	};

    // Inherited via Debugable
    virtual std::string info() const override;
};


inline void Keyboard::handleEvent(const sf::Event& event) noexcept
{
	switch (event.type)
	{
		// Key held down
	case sf::Event::KeyPressed:
	{
		// If key was just held down, increment count.
		if (keys[event.key.code] == false)
		{
			++numberOfKeysHeld;
		}

		// Dodge Tab error.
		if (static_cast<int>(event.key.code) == -1) return;

		// Set key held down to true.
		keys[event.key.code] = true;
		
		break;
	}
	// Key released
	case sf::Event::KeyReleased:
	{
		// Key can't be continuously released,
		// no need for checking.
		--numberOfKeysHeld;

		// Set key held down to false.
		keys[event.key.code] = false;
		break;
	}
	default:	// No key pressed or released
		return;
	}
}

constexpr bool Keyboard::isKeyHeld(sf::Keyboard::Key key) const noexcept
{
	return keys[key];
}

constexpr bool Keyboard::isKeyPressed(sf::Keyboard::Key key) noexcept
{
	auto pressed = keys[key];
	keys[key] = false;
	return pressed;
}

inline std::string Keyboard::info() const
{
	std::stringstream ss;
	size_t evaluated = 0;

	ss << "[KEY PRESSED] {";
	auto size = static_cast<size_t>(sf::Keyboard::KeyCount);
	for (size_t i = 0; i < size; i++)
	{
		if (keys[i])
		{
			evaluated++;
			ss << keyChars[i] << ((evaluated == numberOfKeysHeld) ? "" : " ");
			
		}
	}
	ss << '}';
	return ss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Keyboard& keyboard)
{
	os << "[KEY PRESSED] {";
	for (size_t i = 0; i < static_cast<size_t>(sf::Keyboard::KeyCount); i++)
	{
		if (keyboard.keys[i])
		{
			os << keyboard.keyChars[i] << ' ';
		}
	}
	os << "}";
	return os;
}

