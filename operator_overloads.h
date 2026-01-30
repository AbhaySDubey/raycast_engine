#include "raylib.h"
#include <ostream>


////////////////////
///// VECTOR2 /////
//////////////////

// printing a 2d vector
std::ostream& operator<< (std::ostream &out, const Vector2 &temp);

// multiplying 2d vector by scalar
Vector2 operator*= (Vector2 &a, const float scale);
Vector2 operator* (Vector2 a, const float scale);

// adding a scalar to a 2d vector
Vector2 operator+= (Vector2 &a, const float scale);
Vector2 operator+ (Vector2 a, const float scale);

// multiplying 2 2d vectors
Vector2 operator*= (Vector2 &a, const Vector2 &b);
Vector2 operator* (Vector2 a, const Vector2 &b);

// adding 2 2d vectors
Vector2 operator+= (Vector2 &a, const Vector2 &b);
Vector2 operator+ (Vector2 a, const Vector2 &b);

// subtracting 2 2d vectors
Vector2 operator-= (Vector2 &a, const Vector2 &b);
Vector2 operator- (Vector2 a, const Vector2 &b);

// dividing 2d vector by scalar
Vector2 operator/= (Vector2 &a, const float scale);
Vector2 operator/ (Vector2 a, const float scale);


//////////////////
///// COLOR /////
////////////////

// printing a color
std::ostream& operator<< (std::ostream &out, Color &col);

// comparing equality/inequality of colors
bool operator!= (const Color &c1, const Color &c2);
bool operator== (const Color &c1, const Color &c2);

// dividing color
Color operator/= (Color &c1, int scale);
Color operator/ (Color c1, int scale);

// multiplying color
Color operator*= (Color &c1, int scale);
Color operator* (Color c1, int scale);