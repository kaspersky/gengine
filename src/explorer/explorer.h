#pragma once

namespace explorer {

template <typename IGame, typename Reader, typename Writer, int Size>
void
Explore(const std::string &directory);

}

#include "explorer.hpp"