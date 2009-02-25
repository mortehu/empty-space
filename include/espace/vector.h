#ifndef VECTOR_H_
#define VECTOR_H_

#ifndef SWIG
#include <math.h>
#endif

#include "matrix.h"

/**
 * 2D vector.
 */
struct Vector2
{
  /**
   * Construct an uninitialized vector.
   */
  Vector2()
  {
  }

  /**
   * Construct a vector from two floating point values.
   */
  Vector2(float u, float v)
  {
    _data[0] = u;
    _data[1] = v;
  }

  /**
   * Construct a vector from a 2 element float array.
   */
  Vector2(const float* v)
  {
    _data[0] = v[0];
    _data[1] = v[1];
  }

  /**
   * Copy constructor.
   */
  Vector2(const Vector2& v)
  {
    _data[0] = v._data[0];
    _data[1] = v._data[1];
  }

#ifdef SWIG
  float operator()(int index)
  {
    return _data[index];
  }
#else
  /**
   * Return a specified element in the vector.
   */
  float& operator()(int index)
  {
    return _data[index];
  }

  /**
   * Return a specified element in the vector.
   */
  float operator()(int index) const
  {
    return _data[index];
  }
#endif

  /**
   * Return a float array representation of the vector.
   */
  float* data()
  {
    return _data;
  }

#ifndef SWIG
  /**
   * return a float array representation of the vector.
   */
  const float* data() const
  {
    return _data;
  }
#endif

  /**
   * Return the length of the vector.
   */
  int size() const
  {
    return 2;
  }

  /**
   * Addition operator.
   */
  Vector2 operator+(const Vector2& vec) const
  {
    return Vector2(
      _data[0] + vec._data[0],
      _data[1] + vec._data[1]);
  }

  /**
   * in-place addition operator.
   */
  Vector2& operator+=(const Vector2& vec)
  {
    _data[0] += vec._data[0];
    _data[1] += vec._data[1];

    return *this;
  }

  /**
   * Unary minus operator.
   */
  Vector2 operator-() const
  {
    return Vector2(-_data[0], -_data[1]);
  }

  /**
   * Subtraction operator.
   */
  Vector2 operator-(const Vector2& vec) const
  {
    return Vector2(
      _data[0] - vec._data[0],
      _data[1] - vec._data[1]);
  }

  /**
   * In-place subtraction operator.
   */
  Vector2& operator-=(const Vector2& vec)
  {
    _data[0] -= vec._data[0];
    _data[1] -= vec._data[1];

    return *this;
  }

  /**
   * Dot product operator.
   */
  float operator*(const Vector2& vec) const
  {
    return _data[0] * vec._data[0]
         + _data[1] * vec._data[1];
  }

  /**
   * Multiplication operator.
   */
  Vector2 operator*(float value) const
  {
    return Vector2(
      _data[0] * value,
      _data[1] * value);
  }

  /**
   * In-place multiplication operator.
   */
  Vector2& operator*=(float value)
  {
    _data[0] *= value;
    _data[1] *= value;

    return *this;
  }

  /**
   * In-place vector multiplication operator (not dot or cross product).
   */
  Vector2& operator*=(const Vector2& vec)
  {
    _data[0] *= vec._data[0];
    _data[1] *= vec._data[1];

    return *this;
  }

  /**
   * Division operator.
   */
  Vector2 operator/(float value) const
  {
    return Vector2(
      _data[0] / value,
      _data[1] / value);
  }

  /**
   * In-place division operator.
   */
  Vector2& operator/=(float value)
  {
    _data[0] /= value;
    _data[1] /= value;

    return *this;
  }

  /**
   * In-place vector division operator.
   */
  Vector2& operator/=(const Vector2& vec)
  {
    _data[0] /= vec._data[0];
    _data[1] /= vec._data[1];

    return *this;
  }

  /**
   * Get the spatial length squared.
   */
  float square() const
  {
    return _data[0] * _data[0] + _data[1] * _data[1];
  }

  /**
   * Get the spatial length.
   */
  float magnitude() const
  {
    return sqrt(square());
  }

  /**
   * Normalize the vector.
   */
  Vector2& normalize()
  {
    float mag = magnitude();

    _data[0] /= mag;
    _data[1] /= mag;

    return *this;
  }

  /**
   * Equality operator.
   *
   * \warning floating point numbers should not be tested for equality unless
   * you want to check whether one is a copy of another.
   */
  bool operator==(const Vector2& vec) const
  {
    return (_data[0] == vec._data[0])
        && (_data[1] == vec._data[1]);
  }

protected:

  float _data[2];
};

/**
 * 3D vector.
 */
struct Vector3
{
  /**
   * Construct an uninitialized vector.
   */
  Vector3()
  {
  }

  /**
   * Construct a vector from three floating point values.
   */
  Vector3(float x, float y, float z)
  {
    _data[0] = x;
    _data[1] = y;
    _data[2] = z;
  }

  /**
   * Construct a vector from a 3 element float array.
   */
  Vector3(const float* v)
  {
    _data[0] = v[0];
    _data[1] = v[1];
    _data[2] = v[2];
  }

  /**
   * Copy constructor.
   */
  Vector3(const Vector3& v)
  {
    _data[0] = v._data[0];
    _data[1] = v._data[1];
    _data[2] = v._data[2];
  }

#ifdef SWIG
  float operator()(int index)
  {
    return _data[index];
  }
#else
  /**
   * Return a specified element in the vector.
   */
  float& operator()(int index)
  {
    return _data[index];
  }

  /**
   * Return a specified element in the vector.
   */
  float operator()(int index) const
  {
    return _data[index];
  }
#endif

  /**
   * Return a float array representation of the vector.
   */
  const float* data() const
  {
    return _data;
  }

  /**
   * Return a float array representation of the vector.
   */
  float* data()
  {
    return _data;
  }

  /**
   * Return the length of the vector.
   */
  int size() const
  {
    return 3;
  }

  /**
   * Addition operator.
   */
  Vector3 operator+(const Vector3& vec) const
  {
    return Vector3(
      _data[0] + vec._data[0],
      _data[1] + vec._data[1],
      _data[2] + vec._data[2]);
  }

  /**
   * Addition operator.
   */
  Vector3& operator+=(const Vector3& vec)
  {
    _data[0] += vec._data[0];
    _data[1] += vec._data[1];
    _data[2] += vec._data[2];

    return *this;
  }

  /**
   * Unary minus operator.
   */
  Vector3 operator-() const
  {
    return Vector3(-_data[0], -_data[1], -_data[2]);
  }

  /**
   * Subtraction operator.
   */
  Vector3 operator-(const Vector3& vec) const
  {
    return Vector3(
      _data[0] - vec._data[0],
      _data[1] - vec._data[1],
      _data[2] - vec._data[2]);
  }

  /**
   * In-place subtraction operator.
   */
  Vector3& operator-=(const Vector3& vec)
  {
    _data[0] -= vec._data[0];
    _data[1] -= vec._data[1];
    _data[2] -= vec._data[2];

    return *this;
  }

  /**
   * Dot product operator.
   */
  float operator*(const Vector3& vec) const
  {
    return _data[0] * vec._data[0]
         + _data[1] * vec._data[1]
         + _data[2] * vec._data[2];
  }

  /**
   * Multiplication operator.
   */
  Vector3 operator*(float value) const
  {
    return Vector3(
      _data[0] * value,
      _data[1] * value,
      _data[2] * value);
  }

  /**
   * 3x3 matrix multiplication operator.
   */
  Vector3 operator*(const Matrix3x3& matrix) const
  {
    return Vector3(
        _data[0] * matrix(0, 0)
      + _data[1] * matrix(1, 0)
      + _data[2] * matrix(2, 0),
        _data[0] * matrix(0, 1)
      + _data[1] * matrix(1, 1)
      + _data[2] * matrix(2, 1),
        _data[0] * matrix(0, 2)
      + _data[1] * matrix(1, 2)
      + _data[2] * matrix(2, 2));
  }

  /**
   * 4x4 matrix multiplication operator.
   */
  Vector3 operator*(const Matrix4x4& matrix) const
  {
    return Vector3(
        _data[0] * matrix(0, 0)
      + _data[1] * matrix(1, 0)
      + _data[2] * matrix(2, 0) + matrix(3, 0),
        _data[0] * matrix(0, 1)
      + _data[1] * matrix(1, 1)
      + _data[2] * matrix(2, 1) + matrix(3, 1),
        _data[0] * matrix(0, 2)
      + _data[1] * matrix(1, 2)
      + _data[2] * matrix(2, 2) + matrix(3, 2));
  }

  /**
   * In-place multiplication operator.
   */
  Vector3& operator*=(float value)
  {
    _data[0] *= value;
    _data[1] *= value;
    _data[2] *= value;

    return *this;
  }

  /**
   * In-place vector multiplication operator (not dot or cross product).
   */
  Vector3& operator*=(const Vector3& vec)
  {
    _data[0] *= vec._data[0];
    _data[1] *= vec._data[1];
    _data[2] *= vec._data[2];

    return *this;
  }

  /**
   * In-place 3x3 matrix multiplication operator.
   */
  Vector3& operator*=(const Matrix3x3& matrix)
  {
    float old[2];

    old[0] = _data[0];
    old[1] = _data[1];

    _data[0] = _data[0] * matrix(0, 0)
             + _data[1] * matrix(1, 0)
             + _data[2] * matrix(2, 0);
    _data[1] =   old[0] * matrix(0, 1)
             + _data[1] * matrix(1, 1)
             + _data[2] * matrix(2, 1);
    _data[2] =   old[0] * matrix(0, 2)
             +   old[1] * matrix(1, 2)
             + _data[2] * matrix(2, 2);

    return *this;
  }

  /**
   * In-place 4x4 matrix multiplication operator.
   */
  Vector3& operator*=(const Matrix4x4& matrix)
  {
    float old[2];

    old[0] = _data[0];
    old[1] = _data[1];

    _data[0] = _data[0] * matrix(0, 0)
             + _data[1] * matrix(1, 0)
             + _data[2] * matrix(2, 0) + matrix(3, 0);
    _data[1] =   old[0] * matrix(0, 1)
             + _data[1] * matrix(1, 1)
             + _data[2] * matrix(2, 1) + matrix(3, 1);
    _data[2] =   old[0] * matrix(0, 2)
             +   old[1] * matrix(1, 2)
             + _data[2] * matrix(2, 2) + matrix(3, 2);

    return *this;
  }

  /**
   * Division operator.
   */
  Vector3 operator/(float value) const
  {
    return Vector3(
      _data[0] / value,
      _data[1] / value,
      _data[2] / value);
  }

  /**
   * In-place division operator.
   */
  Vector3& operator/=(float value)
  {
    _data[0] /= value;
    _data[1] /= value;
    _data[2] /= value;

    return *this;
  }

  /**
   * In-place vector division operator.
   */
  Vector3& operator/=(const Vector3& vec)
  {
    _data[0] /= vec._data[0];
    _data[1] /= vec._data[1];
    _data[2] /= vec._data[2];

    return *this;
  }

  /**
   * Cross product operator.
   */
  Vector3 cross(const Vector3& vec) const
  {
    return Vector3(
      _data[1] * vec._data[2] - vec._data[1] * _data[2],
      _data[2] * vec._data[0] - vec._data[2] * _data[0],
      _data[0] * vec._data[1] - vec._data[0] * _data[1]);
  }

  /**
   * Get the spatial length squared.
   */
  float square() const
  {
    return _data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2];
  }

  /**
   * Get the spatial length.
   */
  float magnitude() const
  {
    return sqrt(square());
  }

  /**
   * Normalize the vector.
   */
  Vector3& normalize()
  {
    float mag = magnitude();

    _data[0] /= mag;
    _data[1] /= mag;
    _data[2] /= mag;

    return *this;
  }

  /**
   * Equality operator.
   *
   * \warning floating point numbers should not be tested for equality unless
   * you want to check whether one is a copy of another.
   */
  bool operator==(const Vector3& vec) const
  {
    return (_data[0] == vec._data[0])
        && (_data[1] == vec._data[1])
        && (_data[2] == vec._data[2]);
  }

protected:

  float _data[3];
};

#ifndef SWIG
inline Vector2 operator*(float value, const Vector2& vec)
{
  return vec * value;
}

inline Vector3 operator*(float value, const Vector3& vec)
{
  return vec * value;
}
#endif

#endif // !VECTOR_H_

// vim: ts=2 sw=2 et
