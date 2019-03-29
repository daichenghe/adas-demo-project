///////////////////////////////////////////////////////////////////////////////
// Vectors.h
// =========
// 2D/3D/4D vectors
//
// AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2007-02-14
// UPDATED: 2013-01-20
//
// Copyright (C) 2007-2013 Song Ho Ahn
///////////////////////////////////////////////////////////////////////////////


#ifndef VECTORS_H_DEF
#define VECTORS_H_DEF

#include "avmlib_sdk.h"
#include <math.h>



///////////////////////////////////////////////////////////////////////////////
// 2D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector2
{
    F32 x;
    F32 y;

    // ctors
    Vector2() : x(0), y(0) {};
    Vector2(F32 x, F32 y) : x(x), y(y) {};

    // utils functions
    void        set(F32 x, F32 y);
    F32       length() const;                         //
    F32       distance(const Vector2& vec) const;     // distance between two vectors
    Vector2&    normalize();                            //
    F32       dot(const Vector2& vec) const;          // dot product
    bool        equal(const Vector2& vec, F32 e) const; // compare with epsilon

    // operators
    Vector2     operator-() const;                      // unary operator (negate)
    Vector2     operator+(const Vector2& rhs) const;    // add rhs
    Vector2     operator-(const Vector2& rhs) const;    // subtract rhs
    Vector2&    operator+=(const Vector2& rhs);         // add rhs and update this object
    Vector2&    operator-=(const Vector2& rhs);         // subtract rhs and update this object
    Vector2     operator*(const F32 scale) const;     // scale
    Vector2     operator*(const Vector2& rhs) const;    // multiply each element
    Vector2&    operator*=(const F32 scale);          // scale and update this object
    Vector2&    operator*=(const Vector2& rhs);         // multiply each element and update this object
    Vector2     operator/(const F32 scale) const;     // inverse scale
    Vector2&    operator/=(const F32 scale);          // scale and update this object
    bool        operator==(const Vector2& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector2& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector2& rhs) const;    // comparison for sort
    F32       operator[](S32 index) const;            // subscript operator v[0], v[1]
    F32&      operator[](S32 index);                  // subscript operator v[0], v[1]

    friend Vector2 operator*(const F32 a, const Vector2 vec);
};



///////////////////////////////////////////////////////////////////////////////
// 3D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector3
{
    F32 x;
    F32 y;
    F32 z;

    // ctors
    Vector3() : x(0), y(0), z(0) {};
    Vector3(F32 x, F32 y, F32 z) : x(x), y(y), z(z) {};

    // utils functions
    void        set(F32 x, F32 y, F32 z);
    F32       length() const;                         //
    F32       distance(const Vector3& vec) const;     // distance between two vectors
    F32       angle(const Vector3& vec) const;        // angle between two vectors
    Vector3&    normalize();                            //
    F32       dot(const Vector3& vec) const;          // dot product
    Vector3     cross(const Vector3& vec) const;        // cross product
    bool        equal(const Vector3& vec, F32 e) const; // compare with epsilon

    // operators
    Vector3     operator-() const;                      // unary operator (negate)
    Vector3     operator+(const Vector3& rhs) const;    // add rhs
    Vector3     operator-(const Vector3& rhs) const;    // subtract rhs
    Vector3&    operator+=(const Vector3& rhs);         // add rhs and update this object
    Vector3&    operator-=(const Vector3& rhs);         // subtract rhs and update this object
    Vector3     operator*(const F32 scale) const;     // scale
    Vector3     operator*(const Vector3& rhs) const;    // multiplay each element
    Vector3&    operator*=(const F32 scale);          // scale and update this object
    Vector3&    operator*=(const Vector3& rhs);         // product each element and update this object
    Vector3     operator/(const F32 scale) const;     // inverse scale
    Vector3&    operator/=(const F32 scale);          // scale and update this object
    bool        operator==(const Vector3& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector3& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector3& rhs) const;    // comparison for sort
    F32       operator[](S32 index) const;            // subscript operator v[0], v[1]
    F32&      operator[](S32 index);                  // subscript operator v[0], v[1]

    friend Vector3 operator*(const F32 a, const Vector3 vec);
};



///////////////////////////////////////////////////////////////////////////////
// 4D vector
///////////////////////////////////////////////////////////////////////////////
struct Vector4
{
    F32 x;
    F32 y;
    F32 z;
    F32 w;

    // ctors
    Vector4() : x(0), y(0), z(0), w(0) {};
    Vector4(F32 x, F32 y, F32 z, F32 w) : x(x), y(y), z(z), w(w) {};

    // utils functions
    void        set(F32 x, F32 y, F32 z, F32 w);
    F32       length() const;                         //
    F32       distance(const Vector4& vec) const;     // distance between two vectors
    Vector4&    normalize();                            //
    F32       dot(const Vector4& vec) const;          // dot product
    bool        equal(const Vector4& vec, F32 e) const; // compare with epsilon

    // operators
    Vector4     operator-() const;                      // unary operator (negate)
    Vector4     operator+(const Vector4& rhs) const;    // add rhs
    Vector4     operator-(const Vector4& rhs) const;    // subtract rhs
    Vector4&    operator+=(const Vector4& rhs);         // add rhs and update this object
    Vector4&    operator-=(const Vector4& rhs);         // subtract rhs and update this object
    Vector4     operator*(const F32 scale) const;     // scale
    Vector4     operator*(const Vector4& rhs) const;    // multiply each element
    Vector4&    operator*=(const F32 scale);          // scale and update this object
    Vector4&    operator*=(const Vector4& rhs);         // multiply each element and update this object
    Vector4     operator/(const F32 scale) const;     // inverse scale
    Vector4&    operator/=(const F32 scale);          // scale and update this object
    bool        operator==(const Vector4& rhs) const;   // exact compare, no epsilon
    bool        operator!=(const Vector4& rhs) const;   // exact compare, no epsilon
    bool        operator<(const Vector4& rhs) const;    // comparison for sort
    F32       operator[](S32 index) const;            // subscript operator v[0], v[1]
    F32&      operator[](S32 index);                  // subscript operator v[0], v[1]

    friend Vector4 operator*(const F32 a, const Vector4 vec);
};



// fast math routines from Doom3 SDK
inline F32 invSqrt(F32 x)
{
    F32 xhalf = 0.5f * x;
    int i = *(int*)&x;          // get bits for floating value
    i = 0x5f3759df - (i>>1);    // gives initial guess
    x = *(F32*)&i;            // convert bits back to float
    x = x * (1.5f - xhalf*x*x); // Newton step
    return x;
}



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector2
///////////////////////////////////////////////////////////////////////////////
inline Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}

inline Vector2 Vector2::operator+(const Vector2& rhs) const {
    return Vector2(x+rhs.x, y+rhs.y);
}

inline Vector2 Vector2::operator-(const Vector2& rhs) const {
    return Vector2(x-rhs.x, y-rhs.y);
}

inline Vector2& Vector2::operator+=(const Vector2& rhs) {
    x += rhs.x; y += rhs.y; return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& rhs) {
    x -= rhs.x; y -= rhs.y; return *this;
}

inline Vector2 Vector2::operator*(const F32 a) const {
    return Vector2(x*a, y*a);
}

inline Vector2 Vector2::operator*(const Vector2& rhs) const {
    return Vector2(x*rhs.x, y*rhs.y);
}

inline Vector2& Vector2::operator*=(const F32 a) {
    x *= a; y *= a; return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& rhs) {
    x *= rhs.x; y *= rhs.y; return *this;
}

inline Vector2 Vector2::operator/(const F32 a) const {
    return Vector2(x/a, y/a);
}

inline Vector2& Vector2::operator/=(const F32 a) {
    x /= a; y /= a; return *this;
}

inline bool Vector2::operator==(const Vector2& rhs) const {
    return (x == rhs.x) && (y == rhs.y);
}

inline bool Vector2::operator!=(const Vector2& rhs) const {
    return (x != rhs.x) || (y != rhs.y);
}

inline bool Vector2::operator<(const Vector2& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    return false;
}

inline F32 Vector2::operator[](S32 index) const {
    return (&x)[index];
}

inline F32& Vector2::operator[](S32 index) {
    return (&x)[index];
}

inline void Vector2::set(F32 x, F32 y) {
    this->x = x; this->y = y;
}

inline F32 Vector2::length() const {
    return sqrtf(x*x + y*y);
}

inline F32 Vector2::distance(const Vector2& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y));
}

inline Vector2& Vector2::normalize() {
    //@@const F32 EPSILON = 0.000001f;
    F32 xxyy = x*x + y*y;
    //@@if(xxyy < EPSILON)
    //@@    return *this;

    //F32 invLength = invSqrt(xxyy);
    F32 invLength = 1.0f / sqrtf(xxyy);
    x *= invLength;
    y *= invLength;
    return *this;
}

inline F32 Vector2::dot(const Vector2& rhs) const {
    return (x*rhs.x + y*rhs.y);
}

inline bool Vector2::equal(const Vector2& rhs, F32 epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
}

inline Vector2 operator*(const F32 a, const Vector2 vec) {
    return Vector2(a*vec.x, a*vec.y);
}

// END OF VECTOR2 /////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector3
///////////////////////////////////////////////////////////////////////////////
inline Vector3 Vector3::operator-() const {
    return Vector3(-x, -y, -z);
}

inline Vector3 Vector3::operator+(const Vector3& rhs) const {
    return Vector3(x+rhs.x, y+rhs.y, z+rhs.z);
}

inline Vector3 Vector3::operator-(const Vector3& rhs) const {
    return Vector3(x-rhs.x, y-rhs.y, z-rhs.z);
}

inline Vector3& Vector3::operator+=(const Vector3& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this;
}

inline Vector3 Vector3::operator*(const F32 a) const {
    return Vector3(x*a, y*a, z*a);
}

inline Vector3 Vector3::operator*(const Vector3& rhs) const {
    return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
}

inline Vector3& Vector3::operator*=(const F32 a) {
    x *= a; y *= a; z *= a; return *this;
}

inline Vector3& Vector3::operator*=(const Vector3& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this;
}

inline Vector3 Vector3::operator/(const F32 a) const {
    return Vector3(x/a, y/a, z/a);
}

inline Vector3& Vector3::operator/=(const F32 a) {
    x /= a; y /= a; z /= a; return *this;
}

inline bool Vector3::operator==(const Vector3& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
}

inline bool Vector3::operator!=(const Vector3& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
}

inline bool Vector3::operator<(const Vector3& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    return false;
}

inline F32 Vector3::operator[](S32 index) const {
    return (&x)[index];
}

inline F32& Vector3::operator[](S32 index) {
    return (&x)[index];
}

inline void Vector3::set(F32 x, F32 y, F32 z) {
    this->x = x; this->y = y; this->z = z;
}

inline F32 Vector3::length() const {
    return sqrtf(x*x + y*y + z*z);
}

inline F32 Vector3::distance(const Vector3& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z));
}

inline F32 Vector3::angle(const Vector3& vec) const {
    // return angle between [0, 180]
    F32 l1 = this->length();
    F32 l2 = vec.length();
    F32 d = this->dot(vec);
    F32 angle = acosf(d / (l1 * l2)) / 3.141592f * 180.0f;
    return angle;
}
inline Vector3& Vector3::normalize() {
    //@@const F32 EPSILON = 0.000001f;
    F32 xxyyzz = x*x + y*y + z*z;
    if(xxyyzz <  0.000001)
        return *this; // do nothing if it is ~zero vector

    //F32 invLength = invSqrt(xxyyzz);
    F32 invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline F32 Vector3::dot(const Vector3& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z);
}

inline Vector3 Vector3::cross(const Vector3& rhs) const {
    return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x);
}

inline bool Vector3::equal(const Vector3& rhs, F32 epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon && fabs(z - rhs.z) < epsilon;
}

inline Vector3 operator*(const F32 a, const Vector3 vec) {
    return Vector3(a*vec.x, a*vec.y, a*vec.z);
}

// END OF VECTOR3 /////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// inline functions for Vector4
///////////////////////////////////////////////////////////////////////////////
inline Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

inline Vector4 Vector4::operator+(const Vector4& rhs) const {
    return Vector4(x+rhs.x, y+rhs.y, z+rhs.z, w+rhs.w);
}

inline Vector4 Vector4::operator-(const Vector4& rhs) const {
    return Vector4(x-rhs.x, y-rhs.y, z-rhs.z, w-rhs.w);
}

inline Vector4& Vector4::operator+=(const Vector4& rhs) {
    x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& rhs) {
    x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this;
}

inline Vector4 Vector4::operator*(const F32 a) const {
    return Vector4(x*a, y*a, z*a, w*a);
}

inline Vector4 Vector4::operator*(const Vector4& rhs) const {
    return Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
}

inline Vector4& Vector4::operator*=(const F32 a) {
    x *= a; y *= a; z *= a; w *= a; return *this;
}

inline Vector4& Vector4::operator*=(const Vector4& rhs) {
    x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this;
}

inline Vector4 Vector4::operator/(const F32 a) const {
    return Vector4(x/a, y/a, z/a, w/a);
}

inline Vector4& Vector4::operator/=(const F32 a) {
    x /= a; y /= a; z /= a; w /= a; return *this;
}

inline bool Vector4::operator==(const Vector4& rhs) const {
    return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
}

inline bool Vector4::operator!=(const Vector4& rhs) const {
    return (x != rhs.x) || (y != rhs.y) || (z != rhs.z) || (w != rhs.w);
}

inline bool Vector4::operator<(const Vector4& rhs) const {
    if(x < rhs.x) return true;
    if(x > rhs.x) return false;
    if(y < rhs.y) return true;
    if(y > rhs.y) return false;
    if(z < rhs.z) return true;
    if(z > rhs.z) return false;
    if(w < rhs.w) return true;
    if(w > rhs.w) return false;
    return false;
}

inline F32 Vector4::operator[](S32 index) const {
    return (&x)[index];
}

inline F32& Vector4::operator[](S32 index) {
    return (&x)[index];
}

inline void Vector4::set(F32 x, F32 y, F32 z, F32 w) {
    this->x = x; this->y = y; this->z = z; this->w = w;
}

inline F32 Vector4::length() const {
    return sqrtf(x*x + y*y + z*z + w*w);
}

inline F32 Vector4::distance(const Vector4& vec) const {
    return sqrtf((vec.x-x)*(vec.x-x) + (vec.y-y)*(vec.y-y) + (vec.z-z)*(vec.z-z) + (vec.w-w)*(vec.w-w));
}

inline Vector4& Vector4::normalize() {
    //NOTE: leave w-component untouched
    //@@const F32 EPSILON = 0.000001f;
    F32 xxyyzz = x*x + y*y + z*z;
    //@@if(xxyyzz < EPSILON)
    //@@    return *this; // do nothing if it is zero vector

    //F32 invLength = invSqrt(xxyyzz);
    F32 invLength = 1.0f / sqrtf(xxyyzz);
    x *= invLength;
    y *= invLength;
    z *= invLength;
    return *this;
}

inline F32 Vector4::dot(const Vector4& rhs) const {
    return (x*rhs.x + y*rhs.y + z*rhs.z + w*rhs.w);
}

inline bool Vector4::equal(const Vector4& rhs, F32 epsilon) const {
    return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon &&
           fabs(z - rhs.z) < epsilon && fabs(w - rhs.w) < epsilon;
}

inline Vector4 operator*(const F32 a, const Vector4 vec) {
    return Vector4(a*vec.x, a*vec.y, a*vec.z, a*vec.w);
}

// END OF VECTOR4 /////////////////////////////////////////////////////////////

#endif
