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

#define TO(T,x) (dynamic_cast<T *>(&*(x)))

#define GRID_SIZE 10

//------------------------------------------------------------------------------

#include <string>
#include <set>

#include "base.h"

//! Core module
namespace Core {

using namespace std;
using namespace Base;

//------------------------------------------------------------------------------
//                                Basic types

// Forward declarations

template <class type> class Handle;
class Object; typedef Handle<Object> ObjectHandle;
class Material; typedef Handle<Material> MaterialHandle;
class Player;
typedef int Resource;
typedef float Power;

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
		if (--(ref->count) < 1) delete ref;
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
//                                Material

//! Represents a material
class Material
{
	public:
	//! Selects this material for the next drawing operation
	virtual void select() {}
	//! Clears up the material selection after drawing
	virtual void unselect() {}
	virtual ~Material() {}
};

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

	//! The \ref Material of this object
	MaterialHandle material;

	//! A \ref set of other objects that belong to this object	
	set<ObjectHandle> children;

	//! Creates a new object at \ref Point P with \ref Quaternion "rotation" R
	Object(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>(), MaterialHandle M = Material()) : origin(P), rotation(R), material(M) {}

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

//! Represents a playing person
class Player{
	public:
	typedef unsigned int Id;
	
	//! The ID of this player
	Id id;

	//! The team of this player
	unsigned char team;

	//! The name of this player
	string name;

	//! Constructs a player
	Player(Id _id, unsigned char _team, string _name);
};

//------------------------------------------------------------------------------

} // namespace Core

#endif // _CORE_H

//------------------------------------------------------------------------------
