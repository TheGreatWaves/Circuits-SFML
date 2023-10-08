#pragma once
#ifndef COMMON
#define COMMON

#include <string>

/**
 * Attributes related to saving gate information.
 */
constexpr const char* DEFAULT_GATE_DIRECTORY{ "gates" };
constexpr const char* DEFAULT_RECIPE_SAVE_DIRECTORY{ "sketches" };
constexpr const char* GATE_EXTENSION{ ".gate" };

const std::string SEPERATOR {"/"};
const std::string GATE_RECIPE_DIRECTORY = DEFAULT_GATE_DIRECTORY + SEPERATOR + DEFAULT_RECIPE_SAVE_DIRECTORY + SEPERATOR;

/**
 * Max number of input pins, referring to a pin ID bigger than 
 * this number wi return an output pin.
 */
constexpr std::size_t INPUT_PIN_LIMIT{ 100 };

/**
 * GUI wire's signal speed.
 */
constexpr float WIRE_GROWTH_RATE = 900.f;

#endif /* COMMON */
