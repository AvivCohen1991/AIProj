#pragma once

#include <assert.h>
#include <iostream>
#include <SFML\System\Vector2.hpp>


float norm(const sf::Vector2f & vector);

#ifdef _DEBUG 

#define logout(msg) { std::cout << (msg) << std::endl; }

#else

#define logout(msg)

#endif //_DEBUG