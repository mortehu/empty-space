#ifndef MATRIX_H_
#define MATRIX_H_

#include <math.h>

#include "types.h"

struct Vector3;

/**
 * 3x3 matrix.
 * \author Morten Hustveit
 */
struct Matrix3x3
{
  /**
   * Construct a matrix with undefined values.
   *
   * \see identity()
   */
  Matrix3x3()
  {
  }

  /**
   * Construct a value with values from a float array.
   */
  Matrix3x3(const float* v)
  {
    for(uint i = 0; i < 9; ++i)
      _data[i] = v[i];
  }

  /**
   * Construct a matrix from three vectors.
   */
  Matrix3x3(const Vector3& direction, const Vector3& right, const Vector3& up) IMPORT;

  /**
   * Turn this matrix into the identity matrix.
   */
  void identity()
  {
    for(int i = 0; i < 9; i++)
      _data[i] = (i % 4) ? 0.0 : 1.0;
  }

  /**
   * Invert the matrix by flipping it.
   */
  Matrix3x3 inverse()
  {
    Matrix3x3 ret;

    for(int i = 0; i < 3; ++i)
      for(int j = 0; j < 3; ++j)
        ret(i, j) = (*this)(j, i);

    return ret;
  }

  /**
   * Return the direction part of an orientation matrix.
   */
  IMPORT Vector3 direction() const;

  /**
   * Return the right part of an orientation matrix.
   */
  IMPORT Vector3 right() const;

  /**
   * Return the up part of an orientation matrix.
   */
  IMPORT Vector3 up() const;

#ifdef SWIG
  float operator()(int a, int b)
  {
    return _data[a * 3 + b];
  }
#else
  /**
   * Return the value at the specified coordinate.
   *
   * \todo What is row, and what is column?
   */
  float& operator()(int a, int b)
  {
    return _data[a * 3 + b];
  }

  /**
   * Return the value at the specified coordinate.
   *
   * \todo What is row, and what is column?
   */
  const float operator()(int a, int b) const
  {
    return _data[a * 3 + b];
  }
#endif

  /**
   * Return the data in this matrix as a float array.
   */
  const float* data() const
  {
    return _data;
  }

  /**
   * Return the data in this matrix as a float array.
   */
  float* data()
  {
    return _data;
  }

  /**
   * Return this matrix multiplied with another.
   */
  Matrix3x3 operator*(const Matrix3x3& matrix) const
  {
    Matrix3x3 ret;

    for(int a = 0; a < 3; a++)
      for(int b = 0; b < 3; b++)
      {
        ret(a, b) = (*this)(a, 0) * matrix(0, b)
                  + (*this)(a, 1) * matrix(1, b)
                  + (*this)(a, 2) * matrix(2, b);
      }

    return ret;
  }

  /**
   * Multiply this matrix with another.
   */
  Matrix3x3& operator*=(const Matrix3x3& matrix)
  {
    return (*this = matrix * *this);
  }

  /**
   * Generate a view matrix.
   *
   * \param from The location of the camera.
   * \param at Where the camera is looking at.
   * \param up The up vector.
   */
  Matrix3x3& IMPORT lookAt(const Vector3& from, const Vector3& at,
                           const Vector3& up);

  /**
   * Rotate around an arbitrary axis.
   *
   * \param axis The axis to rotate around.
   * \param angle The angle to rotate (in radians).
   */
  Matrix3x3& IMPORT rotate(const Vector3& axis, float angle);

  /**
   * Scale the matrix.
   */
  Matrix3x3& scale(float x, float y, float z)
  {
    Matrix3x3 tmp;

    tmp.identity();
    tmp(0, 0) = x;
    tmp(1, 1) = y;
    tmp(2, 2) = z;

    return *this *= tmp;
  }

  /**
   * Pitch, yaw and roll the matrix.
   */
  Matrix3x3& pitchYawRoll(float pitch, float yaw, float roll)
  {
    Matrix3x3 tmp;

    float sp = sin(pitch), cp = cos(pitch);
    float sy = sin(yaw),   cy = cos(yaw);
    float sr = sin(roll),  cr = cos(roll);

    tmp.identity();
    tmp(0, 0) = cp * cy;
    tmp(0, 1) = cp * sy,
    tmp(0, 2) = -sp;
    tmp(1, 0) = sr * sp * cy - cr * sy,
    tmp(1, 1) = sr * sp * sy + cr * cy,
    tmp(1, 2) = sr * cp;
    tmp(2, 0) = cr * sp * cy + sr * sy,
    tmp(2, 1) = cr * sp * sy - sr * cy,
    tmp(2, 2) = cr * cp;

    return *this *= tmp;
  }

protected:

  float _data[9];
};

/**
 * 4x4 matrix.
 * \author Morten Hustveit
 */
struct Matrix4x4
{
  /**
   * Construct a matrix with undefined values.
   *
   * \see identity()
   */
  Matrix4x4() { }

  /**
   * Construct a value with values from a float array.
   */
  Matrix4x4(const float* v)
  {
    for(uint i = 0; i < 16; ++i)
      _data[i] = v[i];
  }

  /**
   * Construct a 4x4 matrix from a 3x3 matrix.
   *
   * Values from the identity matrix is used in the undefined parts.
   */
  Matrix4x4(const Matrix3x3& matrix)
  {
    for(int a = 0; a < 3; ++a)
    {
      for(int b = 0; b < 3; ++b)
        (*this)(a, b) = matrix(a, b);

      (*this)(3, a) = 0;
      (*this)(a, 3) = 0;
    }

    (*this)(3, 3) = 1;
  }

  /**
   * Turn this matrix into the identity matrix.
   */
  void identity()
  {
    for(int i = 0; i < 16; ++i)
      _data[i] = (i % 5) ? 0.0 : 1.0;
  }

#ifdef SWIG
  float operator()(int a, int b)
  {
    return _data[a * 4 + b];
  }
#else
  /**
   * Return the value at the specified coordinate.
   *
   * \todo What is row, and what is column?
   */
  float& operator()(int a, int b)
  {
    return _data[a * 4 + b];
  }

  /**
   * Return the value at the specified coordinate.
   *
   * \todo What is row, and what is column?
   */
  const float operator()(int a, int b) const
  {
    return _data[a * 4 + b];
  }
#endif

  /**
   * Return the data in this matrix as a float array.
   */
  const float* data() const
  {
    return _data;
  }

  /**
   * Return the data in this matrix as a float array.
   */
  float* data()
  {
    return _data;
  }

  /**
   * Return this matrix multiplied with another.
   */
  Matrix4x4 operator*(const Matrix4x4& matrix) const
  {
    Matrix4x4 ret;

    for(int a = 0; a < 4; a++)
      for(int b = 0; b < 4; b++)
      {
        ret(a, b) = (*this)(a, 0) * matrix(0, b)
                  + (*this)(a, 1) * matrix(1, b)
                  + (*this)(a, 2) * matrix(2, b)
                  + (*this)(a, 3) * matrix(3, b);
      }

    return ret;
  }

  /**
   * Multiply this matrix with another.
   */
  Matrix4x4& operator*=(const Matrix4x4& matrix)
  {
    return (*this = matrix * *this);
  }

  /**
   * Rotate around an arbitrary axis.
   *
   * \param axis The axis to rotate around.
   * \param angle The angle to rotate (in radians).
   */
  Matrix4x4& IMPORT rotate(const Vector3& axis, float angle);

  /**
   * Scale the matrix.
   */
  Matrix4x4& scale(float x, float y, float z)
  {
    Matrix4x4 tmp;

    tmp.identity();
    tmp(0, 0) = x;
    tmp(1, 1) = y;
    tmp(2, 2) = z;

    return *this *= tmp;
  }

  /**
   * Translate the matrix.
   */
  Matrix4x4& translate(float x, float y, float z)
  {
    Matrix4x4 tmp;

    tmp.identity();
    tmp(3, 0) = x;
    tmp(3, 1) = y;
    tmp(3, 2) = z;

    return *this *= tmp;
  }

protected:	

  float _data[16];
};

#endif // !MATRIX_H_

// vim: ts=2 sw=2 et
