//
// Created by dimka on 6/10/2020.
//

#ifndef SFML_CELESTIALOBJECT_H
#define SFML_CELESTIALOBJECT_H

#include <SFML/Graphics.hpp>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include "Math.h"

using vec2 = sf::Vector2f;
//vec2 operator*(const vec2& vector, const vec2& vector2);

class CelestialObject
{
//    static sf::Texture tex1;
//    static sf::Texture tex2;
//    static sf::Texture tex3;
public:
    sf::CircleShape obj;
    vec2 acceleration;
    float r;
    float m;
    sf::Vector2f pos;

    CelestialObject();

    CelestialObject(vec2 _pos, vec2 _vel, int _r, float _m)
            : acceleration(_vel), r(_r), m(_m)
    {
        obj.setPosition(_pos);
        obj.setRadius(r);
        //obj.setTexture(&tex1);
        obj.setOrigin(obj.getLocalBounds().width / 2,
                      obj.getLocalBounds().height / 2);
        r = _r;
        m = _m;
        pos = _pos;
    }

    float getMass() const
    { return m; }

    void addedAcceleration(sf::Vector2f a)
    { acceleration = acceleration + a; }

    operator sf::Drawable&()
    { return obj; }

    void addMass(float _m)
    { m += _m; }

    void addRadius(float _r)
    {
        r += _r;
        obj.setRadius(r);
    }

    void addedPosition(vec2 vector)
    {
        obj.move(vector);
        pos = obj.getPosition();
    }

    vec2 getAccelerationVec()
    {
        return vec2(obj.getPosition() + acceleration * vec2(20, 20));
    }
};

#endif //SFML_CELESTIALOBJECT_H
