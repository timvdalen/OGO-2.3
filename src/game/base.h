/*******************************************************
 * Base module -- header file                          *
 *                                                     *
 * Author: Ferry Timmers                               *
 *                                                     *
 * Date: 15:16 15-3-2012                               *
 *                                                     *
 * Description: Module that contains base classes and  *
 *              operations like vectors and matrices.  *
 *******************************************************/

/*
 * Vector addition:                          Dot product:
 *   p = p1 + v1                               n = v1 ^ v1
 *
 * Vector subtraction:                       Cross product:
 *   p = p1 + -v1                              v = v1 * v2
 *
 * Multiplication/division with a scalar:    Quaternions:
 *   p = p1 + v1*4                             q = Quaternion(a,b,c,d)
 *   p = p2 + -v2/4                            n = q
 *                                             v = q
 * Vector norm:                                q = Quaternion(n,v)
 *   n = !v1
 *                                           Quaternion operations:
 * Unit vector:                                q2 = ~q1
 *   p = p1 + ~v1                              v = q2 * v1 * -q2
 *   v = ~v2
 *
 */

/*
 * Aliases (using namespace Base::Alias)
 *
 * ...<...>     double    float    dword
 * Point          Pd       Pf       Pl
 * Vector         Vd       Vf       Vl
 * Quaternion     Qd       Qf       Ql
 * Rotation       Rd       Rf       Rl
 * Color          Cd       Cf       Cl
 *
 */

#ifndef _BASE_H
#define _BASE_H

#include <math.h>

#define TYPE_ASSERT(type, assertion) \
	typedef int TA_type_ ## type[(assertion)*2 - 1]

typedef signed char byte;         //!< Signed integer of 1 byte
typedef signed short int word;    //!< Signed integer of 2 byte2
typedef signed long int dword;    //!< Signed integer of at least 4 bytes
typedef unsigned char ubyte;      //!< Unsigned integer of 1 byte
typedef unsigned short int uword; //!< Unsigned integer of 2 bytes
typedef unsigned long int udword; //!< Unsigned integer of at least 4 bytes

TYPE_ASSERT(byte,   sizeof(byte)   == 1);
TYPE_ASSERT(ubyte,  sizeof(ubyte)  == 1);
TYPE_ASSERT(word,   sizeof(word)   == 2);
TYPE_ASSERT(uword,  sizeof(uword)  == 2);
TYPE_ASSERT(dword,  sizeof(dword)  >= 4);
TYPE_ASSERT(udword, sizeof(udword) >= 4);

#define FCLAMP(x) ((x) > 1.0 ? 1.0 : ((x) < 0.0 ? 0.0 : (x)))

#define BASE_ALIAS(x,y) namespace Alias { typedef x<double> y ## d; \
                                          typedef x<float>  y ## f; \
										  typedef x<dword>  y ## l; }

namespace Base {

const double Pi = 3.14159265358979323846264338327950288419716939937510;
const double Rad = 2.0 * Pi;
const double Deg = 360.0;
const double Rad2Deg = Deg / Rad;
const double Deg2Rad = Rad / Deg;

//------------------------------------------------------------------------------

// Forward declarations

template <typename type> struct Point;
template <typename type> struct Vector;

//------------------------------------------------------------------------------
//                                Point

//! A point in 3d space
template <class type>
struct Point
{
	type x, y, z;
	
	Point(const Point &P) : x(P.x), y(P.y), z(P.z) {}
	Point(type X = 0, type Y = 0, type Z = 0) : x(X), y(Y), z(Z) {}
	
	//! Vector addition
	Point operator +(const Vector<type> &V) const { return Point(x+V.x, y+V.y, z+V.z); }
	//! Vector subtraction
	Point operator -(const Vector<type> &V) const { return Point(x-V.x, y-V.y, z-V.z); }
	template <typename T> operator Point<T>() const { return Point<T>((T) x,(T) y,(T) z); }
	
	operator Vector<type>() const { return Vector<type>(x,y,z); }
};

BASE_ALIAS(Point, P)

//------------------------------------------------------------------------------
//                                Vector

//! A vector in 3d space
template <typename type>
struct Vector : public Point<type>
{
	using Point<type>::x;
	using Point<type>::y;
	using Point<type>::z;
	
	Vector(type X = 0, type Y = 0, type Z = 0) : Point<type>(X,Y,Z) {}
	//! Flips the vector
	Vector operator -() const { return Vector(-x,-y,-z); }
	//! Scalar multiplication
	Vector operator *(const type &S) const { return Vector(x*S,y*S,z*S); }
	//! Scalar division
	Vector operator /(const type &S) const { return Vector(x/S,y/S,z/S); }
	
	//! Vector addition
	Vector operator +(const Vector<type> &V) const { return Vector(x+V.x, y+V.y, z+V.z); }
	//! Cross product
	Vector operator *(const Vector<type> &V) const
	{ return Vector(y*V.z - z*V.y,
	                z*V.x - x*V.z,
	                x*V.y - y*V.x); }
	//! Dot product
	type operator ^(const Vector<type> &V) const { return x*V.x + y*V.y + z*V.z; }
	//! Vector norm
	type operator !() const { return sqrt(*this ^ *this); }
	//! Reurns a normalized vector
	Vector operator ~() const { return *this / (!*this); }
};

BASE_ALIAS(Vector, V)

//------------------------------------------------------------------------------
//                                Quaternion

//! A quaternion
template <typename type>
struct Quaternion
{
	type a, b, c, d;
	
	Quaternion(type A = 0, type B = 0, type C = 0, type D = 1) : a(A), b(B), c(C), d(D) {}
	Quaternion(type R, const Vector<type> &V) : a(R), b(V.x), c(V.y), d(V.z) {}
	operator type() const { return a; }
	operator Vector<type>() const { return Vector<type>(b,c,d); }
	
	//! Returns the conjungate
	Quaternion operator -() const { return Quaternion(a,-b,-c,-d); }
	//! Scalar multiplication
	Quaternion operator *(const type &S) const { return Quaternion(a*S,b*S,c*S,d*S); }
	//! Scalar division
	Quaternion operator /(const type &S) const { return Quaternion(a/S,b/S,c/S,d/S); }
	//! Quaternion addition
	Quaternion operator +(const Quaternion &Q) const { return Quaternion(a+Q.a,b+Q.b,c+Q.c,d+Q.d); }
	//! Quaternion multiplication
	Quaternion operator *(const Quaternion &Q) const
	{ return Quaternion(a*Q.a - b*Q.b - c*Q.c - d*Q.d,
		                a*Q.b + b*Q.a + c*Q.d - d*Q.c,
						a*Q.c - b*Q.d + c*Q.a + d*Q.b,
						a*Q.d + b*Q.c - c*Q.b + d*Q.a); }
	//! Vector multiplication
	Vector<type> operator *(const Vector<type> &V) const { return (*this) * Quaternion(0,V) * -(*this); }
	//! Quaternion norm
	type operator !() const { return sqrt(a*a + b*b + c*c + d*d); }
	//! Returns a normalized quaternion
	Quaternion operator ~() const { return *this / !(*this); }
	template <typename T> operator Quaternion<T>() const { return Quaternion<T>((T) a,(T) b,(T) c,(T) d); }
};

BASE_ALIAS(Quaternion, Q)

//------------------------------------------------------------------------------
//                                Rotation
// Unit vector + angle representation for rotations
// Passing values other then angles and unit vectors will yield to unexpected results.

template <typename type>
struct Rotation
{
	type a;
	Vector<type> v;
	
	Rotation(type A = 0, const Vector<type> &V = Vector<type>(0,0,1)) : a(A), v(V) {}
	Rotation(const Quaternion<type> &Q) : a(fmod(acos(Q.a) * 2.0f, Rad)) { v = Q; v = ~v; }
	Rotation(const Vector<type> &U = Vector<type>(0,0,1),
	         const Vector<type> &V = Vector<type>(1,0,0)) : a(acos(V^U)), v(~(V*U)) {}
	operator Quaternion<type>() const
	{
		type f = fmod(a, Rad) * .5f;
		return Quaternion<type>(cos(f), ~v * sin(f));
	}
	Rotation operator ~() const { return Rotation(fmod(a,Rad), ~v); }
	template <typename T> operator Rotation<T>() const { return Rotation<T>((T) a, (Vector<T>) v); }
};

BASE_ALIAS(Rotation, R)

//------------------------------------------------------------------------------

template <typename type>
struct Color
{
	union
	{
		type v[4];
		struct { type r, g, b, a; };
	};
	
	Color(type R = 0, type G = 0, type B = 0, type A = 1)
		: r(FCLAMP(R)), g(FCLAMP(G)), b(FCLAMP(B)), a(FCLAMP(A)) {}
	
	Color operator *(const type &S) const { return Color(r*S,g*S,b*S,a*S); }
	Color operator /(const type &S) const { return Color(r/S,g/S,b/S,a/S); }
	operator type *() { return &v[0]; }
};

BASE_ALIAS(Color, C)

//------------------------------------------------------------------------------

} /* namespace Base */

#endif /* _BASE_H */

//..............................................................................
