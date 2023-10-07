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
 * GUI wire's signal speed.
 */
constexpr float WIRE_GROWTH_RATE = 900.f;

#endif /* COMMON */
