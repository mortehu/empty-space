/***************************************************************************
                           matrix.cc  -  3x3 and 4x4 matrix classes
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <espace/matrix.h>
#include <espace/vector.h>

Matrix3x3::Matrix3x3(const Vector3& direction, const Vector3& right,
                     const Vector3& up)
{
  (*this)(0, 0) = direction(0);
  (*this)(1, 0) = direction(1);
  (*this)(2, 0) = direction(2);
  (*this)(0, 1) = right(0);
  (*this)(1, 1) = right(1);
  (*this)(2, 1) = right(2);
  (*this)(0, 2) = up(0);
  (*this)(1, 2) = up(1);
  (*this)(2, 2) = up(2);
}

Vector3 Matrix3x3::direction() const
{
  return Vector3((*this)(0, 0), (*this)(1, 0), (*this)(2, 0));
}

Vector3 Matrix3x3::right() const
{
  return Vector3((*this)(0, 1), (*this)(1, 1), (*this)(2, 1));
}

Vector3 Matrix3x3::up() const
{
  return Vector3((*this)(0, 2), (*this)(1, 2), (*this)(2, 2));
}

Matrix3x3& IMPORT Matrix3x3::lookAt(const Vector3& from, const Vector3& at,
                                    const Vector3& up)
{
  Vector3 z = at - from;
  z.normalize();

  Vector3 x = up.cross(z);
  Vector3 y = z.cross(x);
  x.normalize();
  y.normalize();

  return *this *= Matrix3x3(z, x, y);
}

Matrix3x3& Matrix3x3::rotate(const Vector3& axis, float angle)
{
  Matrix3x3 tmp;

  float sa = sin(angle);
  float ca = cos(angle);

  tmp.identity();
  tmp(0, 0) = ca + (1 - ca) * axis(0) * axis(0);
  tmp(1, 0) =      (1 - ca) * axis(0) * axis(1) - sa * axis(2);
  tmp(2, 0) =      (1 - ca) * axis(0) * axis(2) + sa * axis(1);
  tmp(0, 1) =      (1 - ca) * axis(1) * axis(0) + sa * axis(2);
  tmp(1, 1) = ca + (1 - ca) * axis(1) * axis(1);
  tmp(2, 1) =      (1 - ca) * axis(1) * axis(2) - sa * axis(0);
  tmp(0, 2) =      (1 - ca) * axis(2) * axis(0) - sa * axis(1);
  tmp(1, 2) =      (1 - ca) * axis(2) * axis(1) + sa * axis(0);
  tmp(2, 2) = ca + (1 - ca) * axis(2) * axis(2);

  return *this *= tmp;
}

Matrix4x4& Matrix4x4::rotate(const Vector3& axis, float angle)
{
  Matrix4x4 tmp;

  float sa = sin(angle);
  float ca = cos(angle);

  tmp.identity();
  tmp(0, 0) = ca + (1 - ca) * axis(0) * axis(0);
  tmp(1, 0) =      (1 - ca) * axis(0) * axis(1) - sa * axis(2);
  tmp(2, 0) =      (1 - ca) * axis(0) * axis(2) + sa * axis(1);
  tmp(0, 1) =      (1 - ca) * axis(1) * axis(0) + sa * axis(2);
  tmp(1, 1) = ca + (1 - ca) * axis(1) * axis(1);
  tmp(2, 1) =      (1 - ca) * axis(1) * axis(2) - sa * axis(0);
  tmp(0, 2) =      (1 - ca) * axis(2) * axis(0) - sa * axis(1);
  tmp(1, 2) =      (1 - ca) * axis(2) * axis(1) + sa * axis(0);
  tmp(2, 2) = ca + (1 - ca) * axis(2) * axis(2);

  return *this *= tmp;
}

