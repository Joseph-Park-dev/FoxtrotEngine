// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// https://github.com/gameprogcpp/code
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Math related classes, especially Vectors and their operations.
/// </summary>

#pragma once
#include <cmath>
#include <memory.h>
#include <limits>
#include <assert.h>

#include "MathExports.h"

namespace Math
{
	const float Pi(3.1415926535f);
	const float TwoPi(Math::Pi * 2.0f);
	const float PiOver2(Math::Pi / 2.0f);
	const float Infinity(std::numeric_limits<float>::infinity());
	const float NegInfinity(-std::numeric_limits<float>::infinity());

	/// @brief Converts an angle from degrees to radians.
	/// @param degrees Angle in degrees.
	/// @return Equivalent angle in radians.
	inline float ToRadians(float degrees)
	{
		return degrees * Pi / 180.0f;
	}

	/// @brief Converts an angle from radians to degrees.
	/// @param radians Angle in radians.
	/// @return Equivalent angle in degrees.
	inline float ToDegrees(float radians)
	{
		return radians * 180.0f / Pi;
	}

	/// @brief Tests whether the absolute value is within an inclusive tolerance of zero.
	/// @param val Value to test.
	/// @param epsilon Nonnegative absolute tolerance.
	/// @return True when the absolute value is at most epsilon.
	inline bool NearZero(float val, float epsilon = 0.001f)
	{
		if (fabs(val) <= epsilon)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// @brief Computes the nonnegative magnitude of a scalar.
	/// @param value Input scalar.
	/// @return The nonnegative magnitude of a scalar.
	inline float Abs(float value)
	{
		return fabs(value);
	}

	/// @brief Selects the smaller of two unsigned sizes.
	/// @param a First operand or interpolation start.
	/// @param b Second operand or interpolation end.
	/// @return The smaller of two unsigned sizes.
	inline size_t Min(size_t a, size_t b)
	{
		return a > b ? b : a;
	}

	/// @brief Evaluates the cosine of an angle in radians.
	/// @param angle Angle in radians.
	/// @return The cosine of an angle in radians.
	inline float Cos(float angle)
	{
		return cosf(angle);
	}

	/// @brief Evaluates the sine of an angle in radians.
	/// @param angle Angle in radians.
	/// @return The sine of an angle in radians.
	inline float Sin(float angle)
	{
		return sinf(angle);
	}

	/// @brief Evaluates the tangent of an angle in radians.
	/// @param angle Angle in radians.
	/// @return The tangent of an angle in radians.
	inline float Tan(float angle)
	{
		return tanf(angle);
	}

	/// @brief Computes the principal inverse cosine in radians.
	/// @param value Input scalar.
	/// @return The principal inverse cosine in radians.
	/// @pre value must be in [-1, 1].
	inline float Acos(float value)
	{
		return acosf(value);
	}

	/// @brief Computes the quadrant-aware angle of the point (x, y).
	/// @param y Y component.
	/// @param x X component.
	/// @return The quadrant-aware angle of the point (x, y).
	inline float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}

	/// @brief Evaluates the reciprocal tangent of an angle in radians.
	/// @param angle Angle in radians.
	/// @return The reciprocal tangent of an angle in radians.
	/// @pre The tangent of angle must be nonzero.
	inline float Cot(float angle)
	{
		return 1.0f / Tan(angle);
	}

	/// @brief Interpolates component values from a to b using an unclamped blend factor.
	/// @param a First operand or interpolation start.
	/// @param b Second operand or interpolation end.
	/// @param f Blend factor; zero selects a and one selects b.
	/// @return Component values from a to b using an unclamped blend factor.
	inline float Lerp(float a, float b, float f)
	{
		return a + f * (b - a);
	}

	/// @brief Computes the nonnegative square root.
	/// @param value Input scalar.
	/// @return The nonnegative square root.
	/// @pre value must be nonnegative for a real result.
	inline float Sqrt(float value)
	{
		return sqrtf(value);
	}

	/// @brief Computes the floating-point remainder of division.
	/// @param numer Dividend.
	/// @param denom Nonzero divisor.
	/// @return The floating-point remainder of division.
	inline float Fmod(float numer, float denom)
	{
		return fmod(numer, denom);
	}

	/// @brief Restricts an integer in place to the inclusive interval [min, max].
	/// @param val Value clamped in place.
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @pre min must not exceed max.
	inline void Clamp(int& val, int min, int max)
	{
		if (val < min)
			val = min;
		else if (max < val)
			val = max;
		else if (val == min && val == max)
			val = min;
	}

	/// @brief Restricts a float in place to the inclusive interval [min, max].
	/// @param val Value clamped in place.
	/// @param min Lower bound.
	/// @param max Upper bound.
	/// @pre min must not exceed max.
	inline void Clampf(float& val, float min, float max)
	{
		if (val < min)
			val = min;
		else if (max < val)
			val = max;
		else if (val == min && val == max)
			val = min;
	}

	/// @brief Rounds a positive integer up to the next power of two using bit propagation.
	/// @param m Positive integer with a representable rounded power of two.
	/// @return Smallest representable power of two at least m.
	inline size_t NextPowerOf2(int m)
	{
		m--;
		m |= m >> 1;
		m |= m >> 2;
		m |= m >> 4;
		m |= m >> 8;
		m |= m >> 16;
		m++;
		return m;
	}

	class FTVector2;

	class MATH_API alignas(16) FTVector4
	{
	public:
		union
		{
			__m128 SIMD;
			float  Data[4];
			struct
			{
				float x, y, z, w;
			};
		};

		// Default â€” zero vector
		/// @brief Initializes every vector component to zero.
		FTVector4()
			: SIMD(_mm_setzero_ps()) {}

		// 4 explicit components
		/// @brief Initializes the numeric components from the supplied values.
		/// @param x X component.
		/// @param y Y component.
		/// @param z Z component.
		/// @param w Homogeneous coordinate: one for a position, zero for a direction.
		FTVector4(float x, float y, float z, float w)
			: SIMD(_mm_set_ps(w, z, y, x)) {}

		// Splat â€” all lanes same value
		/// @brief Initializes the numeric components from the supplied values.
		/// @param s Value assigned to every component.
		explicit FTVector4(float s)
			: SIMD(_mm_set1_ps(s)) {}

		// Direct from __m128 (used internally)
		/// @brief Initializes the numeric components from the supplied values.
		/// @param m Positive integer with a representable rounded power of two.
		FTVector4(__m128 m)
			: SIMD(m) {}
	};

	class FTMatrix4;

	// 3D Vector
	class MATH_API FTVector3
	{
	public:
		float x;
		float y;
		float z;

		/// @brief Initializes every vector component to zero.
		FTVector3();
		/// @brief Initializes the numeric components from the supplied values.
		/// @param in Value assigned to every component.
		explicit FTVector3(float in);
		/// @brief Initializes the numeric components from the supplied values.
		/// @param inX X component.
		/// @param inY Y component.
		/// @param inZ Z component.
		explicit FTVector3(float inX, float inY, float inZ);

		/// @brief Copies component values from an existing vector.
		/// @param other Vector whose components are copied.
		FTVector3(const FTVector3& other);	   // copy constructor
		/// @brief Copies component values from an existing vector.
		/// @param other Vector whose components are copied.
		FTVector3(FTVector3&& other) noexcept; // move constructor

		// Vector addition (a + b)
		/// @brief Adds corresponding components.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Corresponding components.
		friend FTVector3 operator+(const FTVector3& a, const FTVector3& b)
		{
			return FTVector3(a.x + b.x, a.y + b.y, a.z + b.z);
		}

		// Vector subtraction (a - b)
		/// @brief Subtracts corresponding components.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Corresponding components.
		friend FTVector3 operator-(const FTVector3& a, const FTVector3& b)
		{
			return FTVector3(a.x - b.x, a.y - b.y, a.z - b.z);
		}

		// Component-wise multiplication
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector3 operator*(const FTVector3& left, const FTVector3& right)
		{
			return FTVector3(left.x * right.x, left.y * right.y, left.z * right.z);
		}

		// Scalar multiplication
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param vec Input vector.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector3 operator*(const FTVector3& vec, float scalar)
		{
			return FTVector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
		}

		// Scalar multiplication
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @param vec Input vector.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector3 operator*(float scalar, const FTVector3& vec)
		{
			return FTVector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
		}

		/// @brief Divides components by the corresponding operand or scalar.
		/// @param vec Input vector.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector3 operator/(const FTVector3& vec, float scalar)
		{
			assert(scalar != 0);
			return FTVector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
		}

		/// @brief Compares all components for exact equality.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return True when the comparison holds; otherwise false.
		friend bool operator==(const FTVector3& left, const FTVector3& right)
		{
			return (left.x == right.x) && (left.y == right.y) && (left.z == right.z);
		}

		/// @brief Tests whether any corresponding components differ.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return True when the comparison holds; otherwise false.
		friend bool operator!=(const FTVector3& left, const FTVector3& right)
		{
			return (left.x != right.x) || (left.y != right.y) || (left.z != right.z);
		}

		/// @brief Copies the operand's components into this object.
		/// @param rhs Source operand.
		/// @return Updated left operand.
		FTVector3& operator=(const FTVector3& rhs) // copy assignment
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;

			return *this; // return *this by convention
		}

		/// @brief Copies the operand's components into this object.
		/// @param rhs Source operand.
		/// @return Updated left operand.
		FTVector3& operator=(FTVector3&& rhs) noexcept // move assignment
		{
			this->x = rhs.x;
			this->y = rhs.y;
			this->z = rhs.z;

			return *this; // return *this by convention
		}

		// Scalar *=
		/// @brief Multiplies the left operand in place.
		/// @param vec3 Input operand.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Updated left operand.
		friend FTVector3 operator*=(FTVector3& vec3, float scalar)
		{
			vec3.x *= scalar;
			vec3.y *= scalar;
			vec3.z *= scalar;
			return vec3;
		}

		// Vector +=
		/// @brief Accumulates the right operand into the left operand.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Updated left operand.
		friend FTVector3 operator+=(FTVector3& left, const FTVector3& right)
		{
			left.x += right.x;
			left.y += right.y;
			left.z += right.z;
			return left;
		}

		// Vector -=
		/// @brief Subtracts the right operand from the left operand in place.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Updated left operand.
		friend FTVector3 operator-=(FTVector3& left, const FTVector3& right)
		{
			left.x -= right.x;
			left.y -= right.y;
			left.z -= right.z;
			return left;
		}

		// Vector *=
		/// @brief Multiplies the left operand in place.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Updated left operand.
		friend FTVector3 operator*=(FTVector3& left, const FTVector3& right)
		{
			left.x *= right.x;
			left.y *= right.y;
			left.z *= right.z;
			return left;
		}

		// Vector /=
		/// @brief Divides the left operand in place.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Updated left operand.
		friend FTVector3 operator/=(FTVector3& left, const float& right)
		{
			left.x /= right;
			left.y /= right;
			left.z /= right;
			return left;
		}

		/// @brief Copies this vector's components into caller-provided output storage.
		/// @param arr Writable storage for three float components.
		void Assign(float arr[3]) const;
		/// @brief Copies this vector's components into caller-provided output storage.
		/// @param out Receives a copy of this vector.
		void Assign(FTVector3& out) const;

		// Length squared of vector
		/// @brief Computes the sum of squared components without taking a square root.
		/// @return Squared Euclidean magnitude.
		float LengthSq();

		// Length of vector
		/// @brief Computes the Euclidean magnitude of the components.
		/// @return Euclidean magnitude.
		float Length();

		// Normalize this vector
		/// @brief Scales this object's components to unit length.
		/// @pre The input magnitude must be nonzero.
		void Normalize();

		// Normalize the provided vector
		/// @brief Produces a unit-length copy of the input.
		/// @param vec Input vector.
		/// @return Unit-length value.
		/// @pre The input magnitude must be nonzero.
		static FTVector3 Normalize(const FTVector3& vec);

		// Dot product between two vectors (a dot b)
		/// @brief Computes the scalar inner product of two component sequences.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Scalar inner product.
		static float Dot(const FTVector3& a, const FTVector3& b);

		// Cross product between two vectors (a cross b)
		/// @brief Computes the right-handed vector perpendicular to the two input vectors.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Cross-product vector.
		static FTVector3 Cross(const FTVector3& a, const FTVector3& b);

		// Lerp from A to B by f
		/// @brief Interpolates component values from a to b using an unclamped blend factor.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @param f Blend factor; zero selects a and one selects b.
		/// @return Component values from a to b using an unclamped blend factor.
		static FTVector3 Lerp(const FTVector3& a, const FTVector3& b, float f);

		// Reflect V about (normalized) N
		/// @brief Reflects a vector across a plane with the supplied unit normal.
		/// @param v Incident vector.
		/// @param n Unit-length surface normal.
		/// @return A vector across a plane with the supplied unit normal.
		/// @pre The normal must have unit length.
		static FTVector3 Reflect(const FTVector3& v, const FTVector3& n);

		/// @brief Applies a row-vector affine matrix transform with the supplied homogeneous coordinate.
		/// @param vec Input vector.
		/// @param mat Row-vector transformation matrix.
		/// @param w Homogeneous coordinate: one for a position, zero for a direction.
		/// @return A row-vector affine matrix transform with the supplied homogeneous coordinate.
		static FTVector3 Transform(const FTVector3& vec, const FTMatrix4& mat, float w /*= 1.0f*/);

		static const FTVector3 Zero;
		static const FTVector3 UnitX;
		static const FTVector3 UnitY;
		static const FTVector3 UnitZ;
		static const FTVector3 NegUnitX;
		static const FTVector3 NegUnitY;
		static const FTVector3 NegUnitZ;
		static const FTVector3 Infinity;
		static const FTVector3 NegInfinity;
	};

	class MATH_API FTVector2
	{
	public:
		union
		{
			float Data[2];
			struct
			{
				float x, y;
			};
		};

		/// @brief Initializes every vector component to zero.
		FTVector2()
			: x(0.0f), y(0.0f) {}

		// 2 components â€” z,w lanes set to 0
		/// @brief Initializes the numeric components from the supplied values.
		/// @param xIn X component.
		/// @param yIn Y component.
		FTVector2(float xIn, float yIn)
			: x(xIn), y(yIn) {}

		// 2 components â€” z,w lanes set to 0
		/// @brief Initializes the numeric components from the supplied values.
		/// @param xIn X component.
		/// @param yIn Y component.
		FTVector2(unsigned int xIn, unsigned int yIn)
			: x(static_cast<float>(xIn)), y(static_cast<float>(yIn)) {}

		// Splat
		/// @brief Initializes the numeric components from the supplied values.
		/// @param s Value assigned to every component.
		explicit FTVector2(float s)
			: x(s), y(s) {}

		// Vector addition (a + b)
		/// @brief Adds corresponding components.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Corresponding components.
		friend FTVector2 operator+(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x + b.x, a.y + b.y);
		}

		// Vector subtraction (a - b)
		/// @brief Subtracts corresponding components.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Corresponding components.
		friend FTVector2 operator-(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x - b.x, a.y - b.y);
		}

		// Component-wise multiplication
		// (a.x * b.x, ...)
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector2 operator*(const FTVector2& a, const FTVector2& b)
		{
			return FTVector2(a.x * b.x, a.y * b.y);
		}

		// Scalar multiplication
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param vec Input vector.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector2 operator*(const FTVector2& vec, float scalar)
		{
			return FTVector2(vec.x * scalar, vec.y * scalar);
		}

		// Scalar multiplication
		/// @brief Multiplies components by the corresponding operand or scalar.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @param vec Input vector.
		/// @return Components by the corresponding operand or scalar.
		friend FTVector2 operator*(float scalar, const FTVector2& vec)
		{
			return FTVector2(vec.x * scalar, vec.y * scalar);
		}

		/// @brief Divides components by the corresponding operand or scalar.
		/// @param vOther Input operand.
		/// @return Components by the corresponding operand or scalar.
		FTVector2 operator/(FTVector2 vOther)
		{
			assert(!(vOther.x == 0.f || vOther.y == 0.f));
			return FTVector2(x / vOther.x, y / vOther.y);
		}

		/// @brief Divides components by the corresponding operand or scalar.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Components by the corresponding operand or scalar.
		FTVector2 operator/(float scalar)
		{
			assert(!(scalar == 0.f));
			return FTVector2(x / scalar, y / scalar);
		}

		/// @brief Divides the left operand in place.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		void operator/=(float scalar)
		{
			assert(!(scalar == 0.f));
			this->x /= scalar;
			this->y /= scalar;
		}

		// Scalar *=
		/// @brief Multiplies the left operand in place.
		/// @param scalar Uniform multiplier or divisor; divisors must be nonzero.
		/// @return Updated left operand.
		FTVector2& operator*=(float scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		/// @brief Multiplies the left operand in place.
		/// @param vec2 Input operand.
		/// @return Updated left operand.
		FTVector2& operator*=(FTVector2 vec2)
		{
			x = x * vec2.x;
			y = y * vec2.y;
			return *this;
		}

		// Vector +=
		/// @brief Accumulates the right operand into the left operand.
		/// @param right Right operand.
		/// @return Updated left operand.
		FTVector2& operator+=(const FTVector2& right)
		{
			x += right.x;
			y += right.y;
			return *this;
		}

		// Vector -=
		/// @brief Subtracts the right operand from the left operand in place.
		/// @param right Right operand.
		/// @return Updated left operand.
		FTVector2& operator-=(const FTVector2& right)
		{
			x -= right.x;
			y -= right.y;
			return *this;
		}

		/// @brief Negates every component.
		/// @return Every component.
		FTVector2 operator-()
		{
			return FTVector2(-x, -y);
		}

		/// @brief Compares all components for exact equality.
		/// @param right Right operand.
		/// @return True when the comparison holds; otherwise false.
		bool operator==(const FTVector2& right)
		{
			return (this->x == right.x) && (this->y == right.y);
		}

		/// @brief Tests whether any corresponding components differ.
		/// @param right Right operand.
		/// @return True when the comparison holds; otherwise false.
		bool operator!=(const FTVector2& right)
		{
			return (this->x != right.x) || (this->y != right.y);
		}

		/// @brief Accumulates a scaled vector into this vector's components.
		/// @param vector Vector to accumulate.
		/// @param scale Uniform scale factor.
		void addScaledVector(const FTVector2& vector, float scale)
		{
			x += vector.x * scale;
			y += vector.y * scale;
		}

		/// @brief Tests whether both components are exactly zero.
		/// @return True when both components equal zero.
		bool IsZero() const
		{
			return (x == 0.f && y == 0.f);
		}

		// Length squared of vector
		/// @brief Computes the sum of squared components without taking a square root.
		/// @return Squared Euclidean magnitude.
		float LengthSq() const
		{
			return (x * x + y * y);
		}

		// Length of vector
		/// @brief Computes the Euclidean magnitude of the components.
		/// @return Euclidean magnitude.
		float Length() const
		{
			return (Math::Sqrt(LengthSq()));
		}

		// Normalize this vector
		/// @brief Scales this object's components to unit length.
		/// @return Unit-length value.
		/// @pre The input magnitude must be nonzero.
		FTVector2 Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			return *this;
		}

		// Normalize the provided vector
		/// @brief Produces a unit-length copy of the input.
		/// @param vec Input vector.
		/// @return Unit-length value.
		/// @pre The input magnitude must be nonzero.
		static FTVector2 Normalize(const FTVector2& vec)
		{
			FTVector2 temp = vec;
			temp.Normalize();
			return temp;
		}

		// Dot product between two vectors (a dot b)
		/// @brief Computes the scalar inner product of two component sequences.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Scalar inner product.
		static float Dot(const FTVector2& a, const FTVector2& b)
		{
			return (a.x * b.x + a.y * b.y);
		}

		// Lerp from A to B by f
		/// @brief Interpolates component values from a to b using an unclamped blend factor.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @param f Blend factor; zero selects a and one selects b.
		/// @return Component values from a to b using an unclamped blend factor.
		static FTVector2 Lerp(const FTVector2& a, const FTVector2& b, float f)
		{
			return FTVector2(a + f * (b - a));
		}

		// Reflect V about (normalized) N
		/// @brief Reflects a vector across a plane with the supplied unit normal.
		/// @param v Incident vector.
		/// @param n Unit-length surface normal.
		/// @return A vector across a plane with the supplied unit normal.
		/// @pre The normal must have unit length.
		static FTVector2 Reflect(const FTVector2& v, const FTVector2& n)
		{
			return v - 2.0f * FTVector2::Dot(v, n) * n;
		}

		/// @brief Converts a polar angle and distance to a Cartesian displacement.
		/// @param angle Angle in radians.
		/// @param dist Radial distance.
		/// @return A polar angle and distance to a Cartesian displacement.
		static FTVector2 PlotCircularFall(float angle, float dist);

		static const FTVector2 Zero;
		static const FTVector2 UnitX;
		static const FTVector2 UnitY;
		static const FTVector2 NegUnitX;
		static const FTVector2 NegUnitY;

		// Transform vector by matrix
		/// @brief Applies a row-vector affine matrix transform with the supplied homogeneous coordinate.
		/// @param vec Input vector.
		/// @param mat Row-vector transformation matrix.
		/// @param w Homogeneous coordinate: one for a position, zero for a direction.
		/// @return A row-vector affine matrix transform with the supplied homogeneous coordinate.
		static FTVector2 Transform(const FTVector2& vec, const class FTMatrix3& mat, float w = 1.0f);

		// SIMD
		/// @brief Adds two pairs of two-dimensional vectors with one SIMD operation.
		/// @param a0 First pair's left operand.
		/// @param b0 First pair's right operand.
		/// @param a1 Second pair's left operand.
		/// @param b1 Second pair's right operand.
		/// @param out0 Receives the result for the first vector pair.
		/// @param out1 Receives the result for the second vector pair.
		static inline void Add_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits â†’ [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction â€” adds all 4 floats simultaneously
			// [X0+X0, Y0+Y0, X1+X1, Y1+Y1]
			__m128 res = _mm_add_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits â†’ out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits â†’ out1
		}

		/// @brief Subtracts two pairs of two-dimensional vectors with one SIMD operation.
		/// @param a0 First pair's left operand.
		/// @param b0 First pair's right operand.
		/// @param a1 Second pair's left operand.
		/// @param b1 Second pair's right operand.
		/// @param out0 Receives the result for the first vector pair.
		/// @param out1 Receives the result for the second vector pair.
		static inline void Sub_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits â†’ [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction â€” subtracts all 4 floats simultaneously
			// [X0+X0, Y0+Y0, X1+X1, Y1+Y1]
			__m128 res = _mm_sub_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits â†’ out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits â†’ out1
		}

		/// @brief Multiplies two pairs of two-dimensional vectors component by component using SIMD.
		/// @param a0 First pair's left operand.
		/// @param b0 First pair's right operand.
		/// @param a1 Second pair's left operand.
		/// @param b1 Second pair's right operand.
		/// @param out0 Receives the result for the first vector pair.
		/// @param out1 Receives the result for the second vector pair.
		static inline void Mul_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits â†’ [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction â€” multiplies all 4 floats simultaneously
			__m128 res = _mm_mul_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits â†’ out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits â†’ out1
		}

		/// @brief Divides two pairs of two-dimensional vectors component by component using SIMD.
		/// @param a0 First pair's left operand.
		/// @param b0 First pair's right operand.
		/// @param a1 Second pair's left operand.
		/// @param b1 Second pair's right operand.
		/// @param out0 Receives the result for the first vector pair.
		/// @param out1 Receives the result for the second vector pair.
		static inline void Div_Vec_X2(
			const FTVector2& a0, const FTVector2& b0, const FTVector2& a1, const FTVector2& b1, FTVector2& out0, FTVector2& out1)
		{
			// Load a0 into low 64 bits, a1 into high 64 bits â†’ [X0, Y0, X1, Y1]
			__m128 a = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)a1.Data));

			__m128 b = _mm_movelh_ps(
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b0.Data),
				_mm_loadl_pi(_mm_setzero_ps(), (__m64*)b1.Data));

			// One instruction â€” divides all 4 floats simultaneously
			__m128 res = _mm_div_ps(a, b);

			// Unpack back to two FTVector2s
			_mm_storel_pi((__m64*)out0.Data, res); // low  64 bits â†’ out0
			_mm_storeh_pi((__m64*)out1.Data, res); // high 64 bits â†’ out1
		}
	};

	// 3x3 Matrix
	class FTMatrix3
	{
	public:
		float mat[3][3];

		/// @brief Initializes an identity transform.
		FTMatrix3()
		{
			*this = FTMatrix3::Identity;
		}

		/// @brief Initializes the numeric components from the supplied values.
		/// @param inMat Row-major source matrix; all components are copied.
		explicit FTMatrix3(float inMat[3][3])
		{
			memcpy(mat, inMat, 9 * sizeof(float));
		}

		// Cast to a const float pointer
		/// @brief Exposes contiguous row-major matrix components for graphics API uploads.
		/// @return Borrowed pointer valid for the lifetime of this matrix.
		const float* GetAsFloatPtr() const
		{
			return reinterpret_cast<const float*>(&mat[0][0]);
		}

		// Matrix multiplication
		/// @brief Multiplies matrices in row-vector composition order.
		/// @param left Left operand.
		/// @param right Right operand.
		/// @return Matrices in row-vector composition order.
		friend FTMatrix3 operator*(const FTMatrix3& left, const FTMatrix3& right)
		{
			FTMatrix3 retVal;
			// row 0
			retVal.mat[0][0] =
				left.mat[0][0] * right.mat[0][0] +
				left.mat[0][1] * right.mat[1][0] +
				left.mat[0][2] * right.mat[2][0];

			retVal.mat[0][1] =
				left.mat[0][0] * right.mat[0][1] +
				left.mat[0][1] * right.mat[1][1] +
				left.mat[0][2] * right.mat[2][1];

			retVal.mat[0][2] =
				left.mat[0][0] * right.mat[0][2] +
				left.mat[0][1] * right.mat[1][2] +
				left.mat[0][2] * right.mat[2][2];

			// row 1
			retVal.mat[1][0] =
				left.mat[1][0] * right.mat[0][0] +
				left.mat[1][1] * right.mat[1][0] +
				left.mat[1][2] * right.mat[2][0];

			retVal.mat[1][1] =
				left.mat[1][0] * right.mat[0][1] +
				left.mat[1][1] * right.mat[1][1] +
				left.mat[1][2] * right.mat[2][1];

			retVal.mat[1][2] =
				left.mat[1][0] * right.mat[0][2] +
				left.mat[1][1] * right.mat[1][2] +
				left.mat[1][2] * right.mat[2][2];

			// row 2
			retVal.mat[2][0] =
				left.mat[2][0] * right.mat[0][0] +
				left.mat[2][1] * right.mat[1][0] +
				left.mat[2][2] * right.mat[2][0];

			retVal.mat[2][1] =
				left.mat[2][0] * right.mat[0][1] +
				left.mat[2][1] * right.mat[1][1] +
				left.mat[2][2] * right.mat[2][1];

			retVal.mat[2][2] =
				left.mat[2][0] * right.mat[0][2] +
				left.mat[2][1] * right.mat[1][2] +
				left.mat[2][2] * right.mat[2][2];

			return retVal;
		}

		/// @brief Multiplies the left operand in place.
		/// @param right Right operand.
		/// @return Updated left operand.
		FTMatrix3& operator*=(const FTMatrix3& right)
		{
			*this = *this * right;
			return *this;
		}

		// Create a scale matrix with x and y scales
		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param xScale X axis scale.
		/// @param yScale Y axis scale.
		/// @return Constructed transformation matrix.
		static FTMatrix3 CreateScale(float xScale, float yScale)
		{
			float temp[3][3] = {
				{ xScale, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix3(temp);
		}

		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param scaleVector Per-axis scale factors.
		/// @return Constructed transformation matrix.
		static FTMatrix3 CreateScale(const FTVector2& scaleVector)
		{
			return CreateScale(scaleVector.x, scaleVector.y);
		}

		// Create a scale matrix with a uniform factor
		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param scale Uniform scale factor.
		/// @return Constructed transformation matrix.
		static FTMatrix3 CreateScale(float scale)
		{
			return CreateScale(scale, scale);
		}

		// Create a rotation matrix about the Z axis
		// theta is in radians
		/// @brief Builds a row-vector rotation matrix for an angle in radians.
		/// @param theta Rotation angle in radians.
		/// @return Constructed transformation matrix.
		static FTMatrix3 CreateRotation(float theta)
		{
			float temp[3][3] = {
				{ Math::Cos(theta), Math::Sin(theta), 0.0f },
				{ -Math::Sin(theta), Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix3(temp);
		}

		// Create a translation matrix (on the xy-plane)
		/// @brief Builds an affine matrix with translation in its final row.
		/// @param trans Translation displacement.
		/// @return Constructed transformation matrix.
		static FTMatrix3 CreateTranslation(const FTVector2& trans)
		{
			float temp[3][3] = {
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ trans.x, trans.y, 1.0f },
			};
			return FTMatrix3(temp);
		}

		static const FTMatrix3 Identity;
	};

	// 4x4 Matrix
	class MATH_API FTMatrix4
	{
	public:
		float mat[4][4];

		/// @brief Initializes an identity transform.
		FTMatrix4()
		{
			*this = FTMatrix4::Identity;
		}

		/// @brief Initializes the numeric components from the supplied values.
		/// @param inMat Row-major source matrix; all components are copied.
		explicit FTMatrix4(float inMat[4][4])
		{
			memcpy(mat, inMat, 16 * sizeof(float));
		}

		// Cast to a const float pointer
		/// @brief Exposes contiguous row-major matrix components for graphics API uploads.
		/// @return Borrowed pointer valid for the lifetime of this matrix.
		const float* GetAsFloatPtr() const
		{
			return reinterpret_cast<const float*>(&mat[0][0]);
		}

		// Matrix multiplication (a * b)
		/// @brief Multiplies matrices in row-vector composition order.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Matrices in row-vector composition order.
		friend FTMatrix4 operator*(const FTMatrix4& a, const FTMatrix4& b)
		{
			FTMatrix4 retVal;
			// row 0
			retVal.mat[0][0] =
				a.mat[0][0] * b.mat[0][0] +
				a.mat[0][1] * b.mat[1][0] +
				a.mat[0][2] * b.mat[2][0] +
				a.mat[0][3] * b.mat[3][0];

			retVal.mat[0][1] =
				a.mat[0][0] * b.mat[0][1] +
				a.mat[0][1] * b.mat[1][1] +
				a.mat[0][2] * b.mat[2][1] +
				a.mat[0][3] * b.mat[3][1];

			retVal.mat[0][2] =
				a.mat[0][0] * b.mat[0][2] +
				a.mat[0][1] * b.mat[1][2] +
				a.mat[0][2] * b.mat[2][2] +
				a.mat[0][3] * b.mat[3][2];

			retVal.mat[0][3] =
				a.mat[0][0] * b.mat[0][3] +
				a.mat[0][1] * b.mat[1][3] +
				a.mat[0][2] * b.mat[2][3] +
				a.mat[0][3] * b.mat[3][3];

			// row 1
			retVal.mat[1][0] =
				a.mat[1][0] * b.mat[0][0] +
				a.mat[1][1] * b.mat[1][0] +
				a.mat[1][2] * b.mat[2][0] +
				a.mat[1][3] * b.mat[3][0];

			retVal.mat[1][1] =
				a.mat[1][0] * b.mat[0][1] +
				a.mat[1][1] * b.mat[1][1] +
				a.mat[1][2] * b.mat[2][1] +
				a.mat[1][3] * b.mat[3][1];

			retVal.mat[1][2] =
				a.mat[1][0] * b.mat[0][2] +
				a.mat[1][1] * b.mat[1][2] +
				a.mat[1][2] * b.mat[2][2] +
				a.mat[1][3] * b.mat[3][2];

			retVal.mat[1][3] =
				a.mat[1][0] * b.mat[0][3] +
				a.mat[1][1] * b.mat[1][3] +
				a.mat[1][2] * b.mat[2][3] +
				a.mat[1][3] * b.mat[3][3];

			// row 2
			retVal.mat[2][0] =
				a.mat[2][0] * b.mat[0][0] +
				a.mat[2][1] * b.mat[1][0] +
				a.mat[2][2] * b.mat[2][0] +
				a.mat[2][3] * b.mat[3][0];

			retVal.mat[2][1] =
				a.mat[2][0] * b.mat[0][1] +
				a.mat[2][1] * b.mat[1][1] +
				a.mat[2][2] * b.mat[2][1] +
				a.mat[2][3] * b.mat[3][1];

			retVal.mat[2][2] =
				a.mat[2][0] * b.mat[0][2] +
				a.mat[2][1] * b.mat[1][2] +
				a.mat[2][2] * b.mat[2][2] +
				a.mat[2][3] * b.mat[3][2];

			retVal.mat[2][3] =
				a.mat[2][0] * b.mat[0][3] +
				a.mat[2][1] * b.mat[1][3] +
				a.mat[2][2] * b.mat[2][3] +
				a.mat[2][3] * b.mat[3][3];

			// row 3
			retVal.mat[3][0] =
				a.mat[3][0] * b.mat[0][0] +
				a.mat[3][1] * b.mat[1][0] +
				a.mat[3][2] * b.mat[2][0] +
				a.mat[3][3] * b.mat[3][0];

			retVal.mat[3][1] =
				a.mat[3][0] * b.mat[0][1] +
				a.mat[3][1] * b.mat[1][1] +
				a.mat[3][2] * b.mat[2][1] +
				a.mat[3][3] * b.mat[3][1];

			retVal.mat[3][2] =
				a.mat[3][0] * b.mat[0][2] +
				a.mat[3][1] * b.mat[1][2] +
				a.mat[3][2] * b.mat[2][2] +
				a.mat[3][3] * b.mat[3][2];

			retVal.mat[3][3] =
				a.mat[3][0] * b.mat[0][3] +
				a.mat[3][1] * b.mat[1][3] +
				a.mat[3][2] * b.mat[2][3] +
				a.mat[3][3] * b.mat[3][3];

			return retVal;
		}

		/// @brief Multiplies the left operand in place.
		/// @param right Right operand.
		/// @return Updated left operand.
		FTMatrix4& operator*=(const FTMatrix4& right)
		{
			*this = *this * right;
			return *this;
		}

		// Invert the matrix - super slow
		/// @brief Replaces this matrix with its multiplicative inverse.
		/// @pre The matrix must be nonsingular.
		void Invert();

		/// @brief Exchanges matrix rows and columns in place.
		void	  Transpose();
		/// @brief Produces a transposed matrix while retaining this matrix's values.
		/// @return A transposed matrix while retaining this matrix's values.
		FTMatrix4 Transposed() const;

		// Get the translation component of the matrix
		/// @brief Extracts the XYZ translation from the matrix's final row.
		/// @return Translation vector.
		FTVector3 GetTranslation() const
		{
			return FTVector3(mat[3][0], mat[3][1], mat[3][2]);
		}

		// Get the x axis of the matrix (forward)
		/// @brief Extracts and normalizes the matrix's X basis vector.
		/// @return Unit-length basis direction.
		FTVector3 GetXAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[0][0], mat[0][1], mat[0][2]));
		}

		// Get the y axis of the matrix (left)
		/// @brief Extracts and normalizes the matrix's Y basis vector.
		/// @return Unit-length basis direction.
		FTVector3 GetYAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[1][0], mat[1][1], mat[1][2]));
		}

		// Get the Z axis of the matrix (up)
		/// @brief Extracts and normalizes the matrix's Z basis vector.
		/// @return Unit-length basis direction.
		FTVector3 GetZAxis() const
		{
			return FTVector3::Normalize(FTVector3(mat[2][0], mat[2][1], mat[2][2]));
		}

		// Extract the scale component from the matrix
		/// @brief Extracts unsigned axis scales as the lengths of the matrix basis vectors.
		/// @return Nonnegative per-axis scale vector.
		FTVector3 GetScale() const
		{
			FTVector3 retVal;
			retVal.x = FTVector3(mat[0][0], mat[0][1], mat[0][2]).Length();
			retVal.y = FTVector3(mat[1][0], mat[1][1], mat[1][2]).Length();
			retVal.z = FTVector3(mat[2][0], mat[2][1], mat[2][2]).Length();
			return retVal;
		}

		// Create a scale matrix with x, y, and z scales
		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param xScale X axis scale.
		/// @param yScale Y axis scale.
		/// @param zScale Z axis scale.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateScale(float xScale, float yScale, float zScale)
		{
			float temp[4][4] = {
				{ xScale, 0.0f, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f, 0.0f },
				{ 0.0f, 0.0f, zScale, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f }
			};
			return FTMatrix4(temp);
		}

		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param scaleVector Per-axis scale factors.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateScale(const FTVector3& scaleVector)
		{
			return CreateScale(scaleVector.x, scaleVector.y, scaleVector.z);
		}

		// Create a scale matrix with a uniform factor
		/// @brief Builds an affine matrix that scales coordinates along the supplied axes.
		/// @param scale Uniform scale factor.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateScale(float scale)
		{
			return CreateScale(scale, scale, scale);
		}

		// Rotation about x-axis
		/// @brief Builds a row-vector rotation matrix around the X axis.
		/// @param theta Rotation angle in radians.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateRotationX(float theta)
		{
			float temp[4][4] = {
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, Math::Cos(theta), Math::Sin(theta), 0.0f },
				{ 0.0f, -Math::Sin(theta), Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Rotation about y-axis
		/// @brief Builds a row-vector rotation matrix around the Y axis.
		/// @param theta Rotation angle in radians.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateRotationY(float theta)
		{
			float temp[4][4] = {
				{ Math::Cos(theta), 0.0f, -Math::Sin(theta), 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ Math::Sin(theta), 0.0f, Math::Cos(theta), 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Rotation about z-axis
		/// @brief Builds a row-vector rotation matrix around the Z axis.
		/// @param theta Rotation angle in radians.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateRotationZ(float theta)
		{
			float temp[4][4] = {
				{ Math::Cos(theta), Math::Sin(theta), 0.0f, 0.0f },
				{ -Math::Sin(theta), Math::Cos(theta), 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f, 1.0f },
			};
			return FTMatrix4(temp);
		}

		// Create a rotation matrix from a quaternion
		/// @brief Builds a rotation matrix from a quaternion orientation.
		/// @param q Input quaternion.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateFromQuaternion(const class Quaternion& q);

		/// @brief Builds an affine matrix with translation in its final row.
		/// @param trans Translation displacement.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateTranslation(const FTVector3& trans)
		{
			float temp[4][4] = {
				{ 1.0f, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ trans.x, trans.y, trans.z, 1.0f }
			};
			return FTMatrix4(temp);
		}

		/// @brief Builds a left-handed view matrix from an eye position, target, and up direction.
		/// @param eye Camera position.
		/// @param target Point the camera faces.
		/// @param up Up direction, not parallel to the viewing direction.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateLookAt(const FTVector3& eye, const FTVector3& target, const FTVector3& up)
		{
			FTVector3 zaxis = FTVector3::Normalize(target - eye);
			FTVector3 xaxis = FTVector3::Normalize(FTVector3::Cross(up, zaxis));
			FTVector3 yaxis = FTVector3::Normalize(FTVector3::Cross(zaxis, xaxis));
			FTVector3 trans;
			trans.x = -FTVector3::Dot(xaxis, eye);
			trans.y = -FTVector3::Dot(yaxis, eye);
			trans.z = -FTVector3::Dot(zaxis, eye);

			float temp[4][4] = {
				{ xaxis.x, yaxis.x, zaxis.x, 0.0f },
				{ xaxis.y, yaxis.y, zaxis.y, 0.0f },
				{ xaxis.z, yaxis.z, zaxis.z, 0.0f },
				{ trans.x, trans.y, trans.z, 1.0f }
			};
			return FTMatrix4(temp);
		}

		/// @brief Builds a left-handed orthographic projection with a zero-to-one depth range.
		/// @param width Projection width; must be positive.
		/// @param height Projection height; must be positive.
		/// @param nearZ Near clipping distance.
		/// @param farZ Far clipping distance, greater than nearZ.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateOrtho(float width, float height, float nearZ, float farZ)
		{
			float temp[4][4] = {
				{ 2.0f / width, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / height, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f },
				{ 0.0f, 0.0f, nearZ / (nearZ - farZ), 1.0f }
			};
			return FTMatrix4(temp);
		}

		/// @brief Builds a left-handed perspective projection with a zero-to-one depth range.
		/// @param fovY Vertical field of view in radians, strictly between zero and Pi.
		/// @param width Projection width; must be positive.
		/// @param height Projection height; must be positive.
		/// @param nearZ Near clipping distance.
		/// @param farZ Far clipping distance, greater than nearZ.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreatePerspectiveFOV(float fovY, float width, float height, float nearZ, float farZ)
		{
			float yScale	 = Math::Cot(fovY / 2.0f);
			float xScale	 = yScale * height / width;
			float temp[4][4] = {
				{ xScale, 0.0f, 0.0f, 0.0f },
				{ 0.0f, yScale, 0.0f, 0.0f },
				{ 0.0f, 0.0f, farZ / (farZ - nearZ), 1.0f },
				{ 0.0f, 0.0f, -nearZ * farZ / (farZ - nearZ), 0.0f }
			};
			return FTMatrix4(temp);
		}

		// Create "Simple" View-Projection Matrix from Chapter 6
		/// @brief Builds the engine's centered two-dimensional view/projection matrix.
		/// @param width Projection width; must be positive.
		/// @param height Projection height; must be positive.
		/// @return Constructed transformation matrix.
		static FTMatrix4 CreateSimpleViewProj(float width, float height)
		{
			float temp[4][4] = {
				{ 2.0f / width, 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / height, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f, 1.0f }
			};
			return FTMatrix4(temp);
		}

		static const FTMatrix4 Identity;
	};

	// (Unit) Quaternion
	class Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		/// @brief Initializes an identity transform.
		Quaternion()
		{
			*this = Quaternion::Identity;
		}

		// This directly sets the quaternion components --
		// don't use for axis/angle
		/// @brief Initializes the numeric components from the supplied values.
		/// @param inX X component.
		/// @param inY Y component.
		/// @param inZ Z component.
		/// @param inW W component.
		explicit Quaternion(float inX, float inY, float inZ, float inW)
		{
			Set(inX, inY, inZ, inW);
		}

		// Construct the quaternion from an axis and angle
		// It is assumed that axis is already normalized,
		// and the angle is in radians
		/// @brief Constructs an orientation from a unit axis and angle in radians.
		/// @param axis Unit-length rotation axis.
		/// @param angle Angle in radians.
		explicit Quaternion(const FTVector3& axis, float angle)
		{
			float scalar = Math::Sin(angle / 2.0f);
			x			 = axis.x * scalar;
			y			 = axis.y * scalar;
			z			 = axis.z * scalar;
			w			 = Math::Cos(angle / 2.0f);
		}

		// Directly set the internal components
		/// @brief Replaces all four quaternion components with the supplied values.
		/// @param inX X component.
		/// @param inY Y component.
		/// @param inZ Z component.
		/// @param inW W component.
		void Set(float inX, float inY, float inZ, float inW)
		{
			x = inX;
			y = inY;
			z = inZ;
			w = inW;
		}

		/// @brief Negates the vector part of this quaternion in place.
		void Conjugate()
		{
			x *= -1.0f;
			y *= -1.0f;
			z *= -1.0f;
		}

		/// @brief Computes the sum of squared components without taking a square root.
		/// @return Squared Euclidean magnitude.
		float LengthSq() const
		{
			return (x * x + y * y + z * z + w * w);
		}

		/// @brief Computes the Euclidean magnitude of the components.
		/// @return Euclidean magnitude.
		float Length() const
		{
			return Math::Sqrt(LengthSq());
		}

		/// @brief Scales this object's components to unit length.
		/// @pre The input magnitude must be nonzero.
		void Normalize()
		{
			float length = Length();
			x /= length;
			y /= length;
			z /= length;
			w /= length;
		}

		// Normalize the provided quaternion
		/// @brief Produces a unit-length copy of the input.
		/// @param q Input quaternion.
		/// @return Unit-length value.
		/// @pre The input magnitude must be nonzero.
		static Quaternion Normalize(const Quaternion& q)
		{
			Quaternion retVal = q;
			retVal.Normalize();
			return retVal;
		}

		// Linear interpolation
		/// @brief Interpolates component values from a to b using an unclamped blend factor.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @param f Blend factor; zero selects a and one selects b.
		/// @return Component values from a to b using an unclamped blend factor.
		static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float f)
		{
			Quaternion retVal;
			retVal.x = Math::Lerp(a.x, b.x, f);
			retVal.y = Math::Lerp(a.y, b.y, f);
			retVal.z = Math::Lerp(a.z, b.z, f);
			retVal.w = Math::Lerp(a.w, b.w, f);
			retVal.Normalize();
			return retVal;
		}

		/// @brief Computes the scalar inner product of two component sequences.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @return Scalar inner product.
		static float Dot(const Quaternion& a, const Quaternion& b)
		{
			return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
		}

		// Spherical Linear Interpolation
		/// @brief Interpolates unit quaternion orientations along the shorter spherical arc.
		/// @param a First operand or interpolation start.
		/// @param b Second operand or interpolation end.
		/// @param f Blend factor; zero selects a and one selects b.
		/// @return Unit quaternion orientations along the shorter spherical arc.
		static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float f)
		{
			float rawCosm = Quaternion::Dot(a, b);

			float cosom = -rawCosm;
			if (rawCosm >= 0.0f)
			{
				cosom = rawCosm;
			}

			float scale0, scale1;

			if (cosom < 0.9999f)
			{
				const float omega  = Math::Acos(cosom);
				const float invSin = 1.f / Math::Sin(omega);
				scale0			   = Math::Sin((1.f - f) * omega) * invSin;
				scale1			   = Math::Sin(f * omega) * invSin;
			}
			else
			{
				// Use linear interpolation if the quaternions
				// are collinear
				scale0 = 1.0f - f;
				scale1 = f;
			}

			if (rawCosm < 0.0f)
			{
				scale1 = -scale1;
			}

			Quaternion retVal;
			retVal.x = scale0 * a.x + scale1 * b.x;
			retVal.y = scale0 * a.y + scale1 * b.y;
			retVal.z = scale0 * a.z + scale1 * b.z;
			retVal.w = scale0 * a.w + scale1 * b.w;
			retVal.Normalize();
			return retVal;
		}

		// Concatenate
		// Rotate by q FOLLOWED BY p
		/// @brief Combines rotations by applying q followed by p.
		/// @param q Input quaternion.
		/// @param p Rotation applied after q.
		/// @return Rotations by applying q followed by p.
		static Quaternion Concatenate(const Quaternion& q, const Quaternion& p)
		{
			Quaternion retVal;

			// Vector component is:
			// ps * qv + qs * pv + pv x qv
			FTVector3 qv(q.x, q.y, q.z);
			FTVector3 pv(p.x, p.y, p.z);
			FTVector3 newVec = p.w * qv + q.w * pv + FTVector3::Cross(pv, qv);
			retVal.x		 = newVec.x;
			retVal.y		 = newVec.y;
			retVal.z		 = newVec.z;

			// Scalar component is:
			// ps * qs - pv . qv
			retVal.w = p.w * q.w - FTVector3::Dot(pv, qv);

			return retVal;
		}

		static const Quaternion Identity;
	};

	/// @brief Tests inclusive rectangle bounds using projections onto two adjacent edges.
	/// @param point Point to test.
	/// @param v0 First rectangle corner.
	/// @param v1 Adjacent rectangle corner.
	/// @param v2 Corner following v1.
	/// @param v3 Fourth rectangle corner; the implementation uses the first three corners.
	/// @return True when the point lies on or inside the rectangle.
	inline bool PointInRectangle(FTVector2 point, FTVector2 v0, FTVector2 v1, FTVector2 v2, FTVector2 v3)
	{
		FTVector2 AB	  = v1 - v0;
		FTVector2 AM	  = point - v0;
		FTVector2 BC	  = v2 - v1;
		FTVector2 BM	  = point - v1;
		float	  dotABAM = FTVector2::Dot(AB, AM);
		float	  dotABAB = FTVector2::Dot(AB, AB);
		float	  dotBCBM = FTVector2::Dot(BC, BM);
		float	  dotBCBC = FTVector2::Dot(BC, BC);
		return 0 <= dotABAM && dotABAM <= dotABAB && 0 <= dotBCBM && dotBCBM <= dotBCBC;
	}
} // namespace Math
