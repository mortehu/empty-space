#ifndef QUAT_H_
#define QUAT_H_

#ifndef SWIG
#include <math.h>

#include "vector.h"
#include "matrix.h"
#endif

/**
 * Quaternion interface.
 */
struct Quat
{
  Quat()
  {
  }

  Quat(float x, float y, float z, float w)
    : xyz(x, y, z),
      w(w)
  {
  }

  Quat(const Vector3& xyz, float w)
    : xyz(-xyz),
      w(w)
  {
  }

  Quat(const Matrix3x3& m)
    : w(sqrt(1 + m(0, 0) + m(1, 1) + m(2, 2)) / 2)
  {
    xyz = Vector3((m(2, 1) - m(1, 2)) / (4 * w),
                  (m(0, 2) - m(2, 0)) / (4 * w),
                  (m(1, 0) - m(0, 1)) / (4 * w));
  }

  Quat& identity()
  {
    xyz = Vector3(0, 0, 0);
    w = 1;

    return *this;
  }

  Quat inverse() const
  {
    return Quat(-xyz, w);
  }

  Quat antipodal() const
  {
    return Quat(-xyz, -w);
  }

  Quat& rotate(const Vector3& axis, float angle)
  {
    return *this *= Quat(axis * sin(angle / 2), cos(angle / 2));
  }

  Quat operator*(const Quat& quat) const
  {
    return Quat((w * quat.xyz) + (quat.w * xyz) - (xyz.cross(quat.xyz)),
                w * quat.w - (xyz * quat.xyz));
  }

  Quat& operator*=(const Quat& quat)
  {
    float oldw = w;

    w = w * quat.w - (xyz * quat.xyz);
    xyz = (oldw * quat.xyz) + (quat.w * xyz) - (xyz.cross(quat.xyz));

    return *this;
  }

  Quat power(float t) const
  {
    Quat ret;

    float u = acos(w);

    ret.xyz = xyz / sin(u) * sin(u * t);
    ret.w = cos(u * t);

    return ret;
  }

  Quat slerp(const Quat& quat, float t) const
  {
    Quat transition = quat * this->inverse();

    if(transition.w < 0)
      transition = transition.antipodal();

    return transition.power(t) * *this;
  }

  Matrix3x3 matrix() const
  {
    Matrix3x3 ret;

    float x = xyz(0);
    float y = xyz(1);
    float z = xyz(2);

    ret(0, 0) = 1 - 2 * y * y - 2 * z * z;
    ret(0, 1) =     2 * x * y - 2 * z * w;
    ret(0, 2) =     2 * x * z + 2 * y * w;

    ret(1, 0) =     2 * x * y + 2 * z * w;
    ret(1, 1) = 1 - 2 * x * x - 2 * z * z;
    ret(1, 2) =     2 * y * z - 2 * x * w;

    ret(2, 0) =     2 * x * z - 2 * y * w;
    ret(2, 1) =     2 * y * z + 2 * x * w;
    ret(2, 2) = 1 - 2 * x * x - 2 * y * y;

    return ret;
  }

  void matrix(Matrix3x3& ret) const
  {
    float x = xyz(0);
    float y = xyz(1);
    float z = xyz(2);

    ret(0, 0) = 1 - 2 * y * y - 2 * z * z;
    ret(0, 1) =     2 * x * y - 2 * z * w;
    ret(0, 2) =     2 * x * z + 2 * y * w;

    ret(1, 0) =     2 * x * y + 2 * z * w;
    ret(1, 1) = 1 - 2 * x * x - 2 * z * z;
    ret(1, 2) =     2 * y * z - 2 * x * w;

    ret(2, 0) =     2 * x * z - 2 * y * w;
    ret(2, 1) =     2 * y * z + 2 * x * w;
    ret(2, 2) = 1 - 2 * x * x - 2 * y * y;
  }

protected:

  Vector3 xyz;
  float   w;
};

#endif // !QUAT_H_

// vim: ts=2 sw=2 et
