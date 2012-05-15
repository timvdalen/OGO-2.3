/*
 * Core module
 *
 * Date: 01-05-12 15:04
 *
 * Description:
 *
 */

#ifndef _CORE_H
#define _CORE_H

//------------------------------------------------------------------------------

#include <set>

#include "base.h"

//! Core module
namespace Core {

using namespace std;
using namespace Base;

//------------------------------------------------------------------------------

// Forward declarations

template <class type> class Handle;
class Object; typedef Handle<Object> ObjectHandle;
struct Material; typedef Handle<Material> MaterialHandle;
typedef int Resource;
typedef float Power;

//------------------------------------------------------------------------------
//                                Object
//! Represents an object in the game
class Object
{
	public:

	//! The origin of this object
	Point<double> origin;

	//! The rotation of this object
	Quaternion<double> rotation;

	//! A \ref set of other objects that belong to this object	
	set<ObjectHandle> children;
	

	//! Creates a new object at \ref Point P with \ref Quaternion "rotation" R
	Object(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>()) : origin(P), rotation(R) {}

	//! Destroys all children and then terminates
	virtual ~Object() {}

	//! Sets up translations and rotations
	virtual void preRender();

	//! Draw the object
	virtual void draw();

	//! Draws the objects children and pop the translations and rotations
	virtual void postRender();

	//! Renders the object. This calls preRender(), draw() and postRender() in that order
	virtual void render();


};

//------------------------------------------------------------------------------
//                                Material

struct Material
{
	virtual void select() {}
	virtual ~Material() {}
};

//------------------------------------------------------------------------------
//                                Handle
// Provides basic garbage collection

template <class type>
class Handle
{
	struct Reference
	{
		type *obj;
		uword count;
		
		template <class T> Reference(const T &O) : obj(new T(O)), count(1) {}
		
		~Reference() { delete obj; }
	} *ref;
	
	public:
	inline void clear()
	{
		if (!ref) return;
		if (--ref->count < 1) delete ref;
		ref = 0;
	}
	
	inline Handle() : ref(0) {}
	inline Handle(const Handle<type> &H) : ref(H.ref) { if (ref) ++ref->count; }
	inline ~Handle() { clear(); }
	
	inline Handle<type> &operator =(const Handle<type> &H)
	{
		if (&H == this) return *this;
		clear();
		ref = H.ref;
		if (ref) ++ref->count;
		return *this;
	}
	
	template <class T> inline Handle(const T &O) : ref(new Reference(O)) {}
	template <class T> inline type &operator =(const T &O)
	{
		clear();
		ref = new Reference(O);
		return *ref->obj;
	}
	
	inline operator bool() const { return !!ref; }
	inline bool operator !() const { return !ref; }
	inline type &operator *() const { return *ref->obj; }
	inline type *operator ->() const { return ref->obj; }
	inline bool operator <(const Handle<type> &H) const { return ref < H.ref; }
	inline bool operator ==(const Handle<type> &H) const { return ref == H.ref; }
	inline bool operator !=(const Handle<type> &H) const { return ref != H.ref; }
};

//------------------------------------------------------------------------------

} // namespace Core

#endif // _CORE_H

//------------------------------------------------------------------------------
