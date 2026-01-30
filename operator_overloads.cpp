#include "raylib.h"
#include "operator_overloads.h"
#include <ostream>


////////////////////
///// VECTOR2 /////
//////////////////

// printing a 2d vector
std::ostream& operator<< (std::ostream &out, const Vector2 &temp) {
    out << temp.x << "," << temp.y;
    return out;
}

// multiplying 2d vector by scalar
Vector2 operator*= (Vector2 &a, const float scale) {
    a.x *= scale;
    a.y *= scale;
    return a;
}
Vector2 operator* (Vector2 a, const float scale) {
    a *= scale;
    return a;
}

// adding a scalar to a 2d vector
Vector2 operator+= (Vector2 &a, const float scale) {
    a.x += scale;
    a.y += scale;
    return a;
}
Vector2 operator+ (Vector2 a, const float scale) {
    a += scale;
    return a;
}

// multiplying 2 2d vectors
Vector2 operator*= (Vector2 &a, const Vector2 &b) {
    a.x *= b.x;
    a.y *= b.y;
    return a;
}
Vector2 operator* (Vector2 a, const Vector2 &b) {
    a *= b;
    return a;
}

// adding 2 2d vectors
Vector2 operator+= (Vector2 &a, const Vector2 &b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}
Vector2 operator+ (Vector2 a, const Vector2 &b) {
    a += b;
    return a;
}

// subtracting 2 2d vectors
Vector2 operator-= (Vector2 &a, const Vector2 &b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}
Vector2 operator- (Vector2 a, const Vector2 &b) {
    a -= b;
    return a;
}

// dividing 2d vector by scalar
Vector2 operator/= (Vector2 &a, const float scale) {
    a.x /= scale; a.y /= scale;
    return a;
}
Vector2 operator/ (Vector2 a, const float scale) {
    a /= scale;
    return a;
}


//////////////////
///// COLOR /////
////////////////

// printing a color
std::ostream& operator<< (std::ostream &out, Color &col) {
    out << "{" << (int)col.r << "," << (int)col.g << "," << (int)col.b << "," << (int)col.a << "}";
    return out;
}

// comparing equality/inequality of colors
bool operator!= (const Color &c1, const Color &c2) {
    return (c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a);
}
bool operator== (const Color &c1, const Color &c2) {
    return !(c1.r != c2.r || c1.g != c2.g || c1.b != c2.b || c1.a != c2.a);
}

// dividing color
Color operator/= (Color &c1, int scale) {
    c1.r /= scale; c1.g /= scale; c1.b /= scale;
    return c1;
}
Color operator/ (Color c1, int scale) {
    c1 /= scale;
    return c1;
}

// multiplying colors
Color operator*= (Color &c1, int scale) {
    c1.r *= scale; c1.g *= scale; c1.b *= scale;
    return c1;
}
Color operator* (Color c1, int scale) {
    c1 *= scale;
    return c1;
}
