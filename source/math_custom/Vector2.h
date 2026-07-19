#pragma once

#include <cmath>
#include <iostream>
#include <cassert>

enum class Direction {
    CW,  // Clockwise
    CCW  // Counter-Clockwise
};

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

    Vector2 operator/(float scalar) const { 
        assert(scalar > 1e-8f && "Division by zero or near-zero");
        return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }

    Vector2& operator/=(float scalar) {
        assert(scalar > 1e-8f && "Division by zero or near-zero");
        x /= scalar; y /= scalar; return *this; }

    Vector2 operator-() const { return Vector2(-x, -y); };

    bool operator==(const Vector2 & other) const { return  x == other.x && y == other.y;}
    bool operator!=(const Vector2& other ) const { return  !(*this == other); }

    bool nearEqual(const Vector2& other, float epsilon = 1e-5f) const {
        return std::abs(x - other.x) < epsilon &&std::abs(y - other.y) < epsilon;
    }
  
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x *x + y *y; }

    Vector2 normalized() const {
        float len = length();
        assert(len > 1e-8f && "Cannot normalize near-zero vector");
        return Vector2(x / len, y / len);
    }

    Vector2 perpendicular(Direction dir) const {
        if(dir == Direction::CW) {
            return Vector2(y, -x);
        } 
        else if(dir == Direction::CCW) {
            return Vector2(-y, x);
        }
        return Vector2(0, 0); // Should not reach here
    }

  
    float dot(const Vector2& other) const { return x * other.x + y * other.y; }
    float cross(const Vector2& other) { return x * other.y - y * other.x; }

    void print() const { std::cout << "(" << x << ", " << y << ")\n"; }
};

inline Vector2 operator*(float scalar, const Vector2 &other) { return other * scalar; }
