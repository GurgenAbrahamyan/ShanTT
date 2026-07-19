#pragma once

#include <cmath>
#include <iostream>

class Vector2 {
public:
    float x, y;

   
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}


    Vector2(const Vector2& other) : x(other.x), y(other.y) {}


    Vector2& operator=(const Vector2& other) {
        x = other.x; y = other.y;
        return *this;
    }

   
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    bool operator==(const Vector2 & other) const { return  x == other.x && y == other.y;}
    bool operator!=(const Vector2& other ) const { return  !(*this == other); }

    bool nearEqual(const Vector2& other, float epsilon) const {
        return std::abs(x - other.x) < epsilon &&std::abs(y - other.y) < epsilon;
    }


  
    float length() const { return std::sqrt(x * x + y * y); }
    Vector2 normalized() const {
        float len = length();
        return (len == 0) ? Vector2(0, 0) : Vector2(x / len, y / len);
    }

  
    float dot(const Vector2& other) const { return x * other.x + y * other.y; }

    void print() const { std::cout << "(" << x << ", " << y << ")\n"; }
};
