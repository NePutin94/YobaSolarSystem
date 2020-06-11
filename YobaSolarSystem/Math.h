#pragma once
#include <iostream>
#include <cmath>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <array>

template<class T, class T2>
bool operator<(const sf::Vector2<T>& v1, const sf::Vector2<T2>& v2)
{
    return (v1.x < v2.x) && (v1.y < v2.y);
}

template<class T, class T2>
bool operator>(const sf::Vector2<T>& v1, const sf::Vector2<T2>& v2)
{
    return (v1.x > v2.x) && (v1.y > v2.y);
}

template<class T, class T2>
auto operator*(const sf::Vector2<T>& v1, const sf::Vector2<T2>& v2) -> sf::Vector2<std::common_type_t<T, T2>>
{
    return { v1.x * v2.x, v1.y * v2.y };
}

template<class T, class T2>
auto operator*(const sf::Vector2<T>& v1, T2 value) -> sf::Vector2<std::common_type_t<T, T2>>
{
    return { v1.x * value, v1.y * value };
}

template<class T, class T2>
auto operator+(const sf::Vector2<T>& v1, const sf::Vector2<T2>& v2) ->  sf::Vector2<std::common_type_t<T, T2>>
{
    return { v1.x + v2.x, v1.y + v2.y };
}

template<class T, class T2>
auto operator-(const sf::Vector2<T>& v1, const sf::Vector2<T2>& v2) ->  sf::Vector2<std::common_type_t<T, T2>>
{
    return v1 + v2 * -1;
}

template<class T, class T2>
sf::Vector2<T2> operator+(const sf::Vector2<T>& vec, T2 value)
{
    return { vec.x + value, vec.y + value };
}

template<class T, class T2>
sf::Vector2<T2> operator-(const sf::Vector2<T>& vec, T2 value)
{
    return vec + value * -1;
}

template<class T, class T2>
sf::Rect<T2> operator*(const sf::Rect<T>& rect, T2 value)
{
    return { rect.left * value, rect.top * value,rect.width * value, rect.height * value };
}
sf::Vector2f VectorAbs(sf::Vector2f vec);

template<class T>
sf::Vector2<T> rotateBy(const sf::Vector2<T>& vec, float degrees, const sf::Vector2<T>& center)
{
    degrees *= 3.14 / 180.f;
    const float cs = cos(degrees);
    const float sn = sin(degrees);

    vec.x -= center.x;
    vec.y -= center.y;

    T nx = (T)(vec.x * cs - vec.y * sn);
    T ny = (T)(vec.x * sn + vec.y * cs);

    vec.x = nx;
    vec.y = ny;

    vec.x += center.x;
    vec.y += center.y;
    return vec;
}

template<class T>
T getAngle(const sf::Vector2<T>& vec)
{
    if (vec.y == 0)
        return vec.x < 0 ? 180 : 0;
    else if (vec.x == 0)
        return vec.y < 0 ? 270 : 90;
    auto pi = 3.14;
    if (vec.y > 0)
        if (vec.x > 0)
            return atan(vec.y / vec.x) * 180.f / pi;
        else
            return 180.0 - atan(vec.y / -vec.x) * 180.f / pi;
    else
    if (vec.x > 0)
        return 360.0 - atan(-vec.y / vec.x) * 180.f / pi;
    else
        return 180.0 + atan(-vec.y / -vec.x) * 180.f / pi;
}

inline float dot(sf::Vector2f v1)
{
    return v1.x * v1.x + v1.y * v1.y;
}

template<class T>
T lenght(const sf::Vector2<T>& v1)
{
    return sqrt(v1.x * v1.x + v1.y * v1.y);
}

template<class T>
inline sf::Vector2<T> perp(sf::Vector2<T> v1)
{
    return { v1.y, -v1.x };
}

template<class T>
inline sf::Vector2<T> normalize(const sf::Vector2<T>& v)
{
    auto l = lenght(v);
    return { v.x / l, v.y / l };
}

template<class T, class T2>
inline std::common_type_t<T, T2> Distance(const sf::Vector2<T>& v2, const sf::Vector2<T2>& v1)
{
    auto delta1 = v2.x - v1.x;
    auto delta2 = v2.y - v1.y;
    return sqrtf(delta1 * delta1 + delta2 * delta2);
}

template<class T>
inline sf::Vector2<T> rotate_point(sf::Vector2<T> center, sf::Vector2<T> point, float angle)
{
    T _sin = sinf(angle);
    T _cos = cosf(angle);
    point.x -= center.x;
    point.y -= center.y;
    T xnew = point.x * _cos - point.y * _sin;
    T ynew = point.x * _sin + point.y * _cos;
    point.x = xnew + center.x;
    point.y = ynew + center.y;
    return point;
}

template<typename T>
inline bool thc(sf::Vector2<T> A, sf::Vector2<T> B, sf::Vector2<T> C)
{
    sf::Vector2<T> a, b;
    a = B - A;
    b = C - A;
    const int cr = a.x * b.y - b.x * a.y;
    if ((cr > 0) || (cr < 0) || (a.x * b.x < 0) ||
        (a.y * b.y < 0) || (a.x * a.x + a.y * a.y < b.x * b.x + b.y * b.y))
        return false;
    return true;
}

template<typename T>
inline T deg2rad(T angle)
{
    return angle *3.14 / (T)180.0;
}

template<class T>
inline auto angleVector(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
    const T a = v1.x - v2.x;
    const T v = v1.y - v2.y;
    T Radian = atan2f(v, a);
    T CurrAngle = (T)(atan2f(v, a)) * 180.f / 3.14;
    if (CurrAngle > 180)
        CurrAngle -= 360;
    if (CurrAngle < -180)
        CurrAngle += 360;
    return CurrAngle;
}

template<class T>
inline sf::Vector2<T> rotateLine(sf::Vector2<T> start, sf::Vector2<T> end, float degrees)
{
    float rotation = deg2rad(degrees - angleVector(start, end));
    return rotate_point(end, start, rotation);
}

template<class LineA, class LineB>
inline auto lineLineIntersection(sf::Vector2<LineA> A, sf::Vector2<LineA> B, sf::Vector2<LineB> C, sf::Vector2<LineB> D)
{
    using resultType = std::common_type_t<LineA, LineB>;
    resultType a1 = B.y - A.y;
    resultType b1 = A.x - B.x;
    resultType c1 = a1 * (A.x) + b1 * (A.y);

    resultType a2 = D.y - C.y;
    resultType b2 = C.x - D.x;
    resultType c2 = a2 * (C.x) + b2 * (C.y);

    auto determinant = a1 * b2 - a2 * b1;
    if (determinant == 0)
        return sf::Vector2<resultType>(-1, -1);
    else
    {
        auto x = (b2 * c1 - b1 * c2) / determinant;
        auto y = (a1 * c2 - a2 * c1) / determinant;
        return sf::Vector2<resultType>(x, y);
    }
}

template<typename T>
inline auto _checkIntersects(sf::Vector2<T> pos1, sf::Vector2<T> pos2, sf::Vector2<T> pos3, sf::Vector2<T> pos4)
{
    sf::Vector2<T> check;
    check = lineLineIntersection(pos1, pos2, pos3, pos4);
    if (thc(pos3, pos4, check) && thc(pos1, pos2, check))
        return check;
    else
        return sf::Vector2<T>(-1, -1);
}