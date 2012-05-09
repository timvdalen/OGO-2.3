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

#include "common.h"
#include "base.h"

namespace Core {

using namespace std;
using namespace Base;

//------------------------------------------------------------------------------

// Forward declarations

template <class type> class Handle;
struct Object; typedef Handle<Object> ObjectHandle;
struct Material; typedef Handle<Material> MaterialHandle;

//------------------------------------------------------------------------------
//                                Object

struct Object
{
	Point<double> origin;
	set<ObjectHandle> children;
	
	Object(Point<double> P = Point<double>()) : origin(P) {}
	virtual ~Object() {}
	
	virtual void preRender();
	virtual void render();
	virtual void postRender();
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
