#include "utils.hpp"
#include <cmath>


float norm(const sf::Vector2f & vector) {
	return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}
