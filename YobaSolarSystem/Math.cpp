#include "Math.h"
#include <map>

sf::Vector2f VectorAbs(sf::Vector2f vec)
{
    sf::Vector2f absVec;
    absVec.x = std::abs(vec.x);
    absVec.y = std::abs(vec.x);
    return absVec;
}

