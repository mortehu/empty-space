#ifndef COLOR_H_
#define COLOR_H_

#ifndef SWIG
#include <stdint.h>
#endif

/**
 * Structure for representing RGBA colors.
 *
 * The member functions can deal with both floating point and integer data.
 * Floating point values must always be in the range [0.0, 1.0], and integer
 * data in the range [0, 255].
 * \author Morten Hustveit
 */
struct Color
{
  /**
   * A mapping between color planes and array indices.
   */
	enum Component
	{
		Red = 0,
    Green = 1,
    Blue = 2,
    Alpha = 3
	};

  Color() { }

  /**
   * Constructs a color object.
   */
	Color(int red, int green, int blue, int alpha = 255)
	{
		_data[Red] =   red;
		_data[Green] = green;
		_data[Blue] =  blue;
		_data[Alpha] = alpha;
	}

  /**
   * Constructs a color object.
   */
	Color(double red, double green, double blue, double alpha = 1.0)
	{
		_data[Red] =   static_cast<uint8_t>(red * 255);
		_data[Green] = static_cast<uint8_t>(green * 255);
		_data[Blue] =  static_cast<uint8_t>(blue * 255);
		_data[Alpha] = static_cast<uint8_t>(alpha * 255);
	}

  /**
   * Constructs a color object from an array of floats.
   *
   * The components are stored in RGBA order.
   */
  Color(const float* v)
  {
    _data[Red] =   static_cast<uint8_t>(v[0] * 255);
    _data[Green] = static_cast<uint8_t>(v[1] * 255);
    _data[Blue] =  static_cast<uint8_t>(v[2] * 255);
    _data[Alpha] = static_cast<uint8_t>(v[3] * 255);
  }

  /**
   * Constructs a color object from an array of doubles.
   *
   * The components are stored in RGBA order.
   */
  Color(const double* v)
  {
    _data[Red] =   static_cast<uint8_t>(v[0] * 255);
    _data[Green] = static_cast<uint8_t>(v[1] * 255);
    _data[Blue] =  static_cast<uint8_t>(v[2] * 255);
    _data[Alpha] = static_cast<uint8_t>(v[3] * 255);
  }

#ifndef SWIG
  /**
   * Returns the value of the specified color component.
   */
	uint8_t operator()(int component) const
	{
		return _data[component];
	}
#endif

  /**
   * Returns the value of the specified color component.
   */
	uint8_t& operator()(int component)
	{
		return _data[component];
	}

#ifndef SWIG
  /**
   * Returns a pointer to a uint8_t array representation of the color.
   *
   * The components are stored in RGBA order.
   */
	const uint8_t* data() const
	{
		return _data;
	}
#endif

  /**
   * Returns a pointer to a uint8_t array representation of the color.
   *
   * The components are stored in RGBA order.
   */
	uint8_t* data()
	{
		return _data;
	}

  /**
   * Adds the non-alpha components from another color.
   *
   * If the sum of any component is larger than 255, the value will overflow.
   */
	Color& operator+=(const Color& color)
	{
		_data[0] += color(0);
		_data[1] += color(1);
		_data[2] += color(2);

    return *this;
	}

  /**
   * Multiplies the non-alpha components from another color.
   */
	Color& operator*=(const Color& color)
	{
		_data[0] = ((static_cast<int>(_data[0]) + 1) * color(0)) >> 8;
		_data[1] = ((static_cast<int>(_data[1]) + 1) * color(1)) >> 8;
		_data[2] = ((static_cast<int>(_data[2]) + 1) * color(2)) >> 8;

    return *this;
	}

  /**
   * Scales color by scalar / 255.
   */
	Color& operator*=(int scalar)
	{
		_data[0] = (static_cast<int>(_data[0]) * (scalar + 1)) >> 8;
		_data[1] = (static_cast<int>(_data[1]) * (scalar + 1)) >> 8;
		_data[2] = (static_cast<int>(_data[2]) * (scalar + 1)) >> 8;

    return *this;
	}

  /**
   * Returns the color scaled by scalar / 255.
   */
	Color operator*(int scalar) const
	{
    return Color(
		  (static_cast<int>(_data[0]) * (scalar + 1)) >> 8,
		  (static_cast<int>(_data[1]) * (scalar + 1)) >> 8,
		  (static_cast<int>(_data[2]) * (scalar + 1)) >> 8);
	}

  /**
   * Sets the value of the red component of the color.
   */
  Color& setRed(float red)
  {
    _data[0] = static_cast<uint8_t>(red * 255);

    return *this;
  }

  /**
   * Sets the value of the red component of the color.
   */
  Color& setGreen(float green)
  {
    _data[1] = static_cast<uint8_t>(green * 255);

    return *this;
  }

  /**
   * Sets the value of the red component of the color.
   */
  Color& setBlue(float blue)
  {
    _data[2] = static_cast<uint8_t>(blue * 255);

    return *this;
  }

  /**
   * Sets the value of the red component of the color.
   */
  Color& setAlpha(float alpha)
  {
    _data[3] = static_cast<uint8_t>(alpha * 255);

    return *this;
  }

protected:
	
	uint8_t _data[4]; /**< The internal representation. */
};

#endif // !COLOR_H_

// vim: ts=2 sw=2 et
