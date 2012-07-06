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

#define AUTOBUILD // This has to be moved, or rather *removed*...

#define TO(T,x) (dynamic_cast<T *>(&*(x)))

#define PRO\
BE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

//------------------------------------------------------------------------------

#include <string>
#include <stack>
#include <map>
#include <set>
#include <vector>

#include "base.h"

//! Core module
namespace Core {

using namespace std;
using namespace Base;
//------------------------------------------------------------------------------
//                                Forward declarations

template <class type> class Handle;
class Object; typedef Handle<Object> ObjectHandle;
class Material; typedef Handle<Material> MaterialHandle;

//------------------------------------------------------------------------------
//                                Basic types

typedef float Resource; //!< Resource type
typedef float Power;    //!< Power type

//------------------------------------------------------------------------------
//                                Conversions

string convert(const Point<double> &);
string convert(const Vector<double> &);
string convert(const Quaternion<double> &);
string convert(const Rotation<double> &);
string convert(const Color<float> &);
string convert(const double &);
string convert(const long &);

Point<double>      ToPoint(const string &);
Vector<double>     ToVector(const string &);
Quaternion<double> ToQuaternion(const string &);
Rotation<double>   ToRotation(const string &);
Color<float>       ToColor(const string &);
double             ToFloat(const string &);
long               ToInteger(const string &);

inline string operator |(const string &lhs, const string &rhs)
	{ return lhs + string("|") + rhs; }

#define NAME(x) virtual string type() const { return #x; }
#define SERIAL(x) virtual operator string() const { return x; }
#define UNSERIAL(arg,num,x)                                                    \
virtual bool unserialize(vector<string> &arg)                                  \
	{ if ((arg.size() < (num)+1) || (arg[0] != type())) return false;          \
	arg.erase(arg.begin()); {x} return true; }
#define REGISTER(x,...) ObjectHandle _ ## x() { return x(__VA_ARGS__); }       \
	Object::Constructor __ ## x(#x,_ ## x);

//------------------------------------------------------------------------------
//                                Handle

//! Handle: Provides basic garbage collection
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
	public: NAME(Object) SERIAL(type() | convert(origin) | convert(rotation))
	UNSERIAL(arg, 2,
		origin   = ToPoint(arg[0]);
		rotation = ToQuaternion(arg[1]);
	)
	
	//! The origin of this object
	Point<double> origin;
	
	//! The rotation of this object
	Quaternion<double> rotation;
	
	//! The origin of this object in world coordinates
	Point<double> absoluteOrigin() { return parentOrigin + parentRotation * origin; }
	//! The rotation of this object in world angle
	Quaternion<double> absoluteRotation() { return parentRotation * rotation; }
	//! Updates the absolute coordinates and rotation of the children
	virtual void updateAbsolute();
	
	//! The \ref Material of this object
	MaterialHandle material;
	
	//! A \ref set of other objects that belong to this object	
	set<ObjectHandle> children;
	
	//! Creates a new object at \ref Point P with \ref Quaternion "rotation" R
	Object(Point<double> P = Point<double>(),
	       Quaternion<double> R = Quaternion<double>(),
		   MaterialHandle M = Material())
	: origin(P), rotation(R), material(M) {}
	
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
	
	//! Unserialize from a string
	bool unserialize(const string &str);
	
	class iterator : public std::iterator<input_iterator_tag,ObjectHandle>
	{
		typedef set<ObjectHandle>::iterator I;
		stack< pair<I,I> > p;
		public:
		size_t level() { return p.size(); }
		iterator() {}
		iterator(const I &begin, const I &end) { p.push(make_pair(begin,end)); }
		iterator(const iterator &it) : p(it.p) {}
		iterator &operator ++()
		{
			if (!(*p.top().first)->children.empty())                 // Push
				p.push(make_pair((*p.top().first)->children.begin(),
				                 (*p.top().first)->children.end()));
			else if (++p.top().first != p.top().second);             // Step
			else                                                     // Pop
				while ((p.top().first == p.top().second) && (p.size() > 1))
					{ p.pop(); ++p.top().first; }
			return *this;
		}
		iterator operator ++(int) { iterator tmp(*this); operator++(); return tmp; }
		bool operator ==(const iterator &rhs) const { return p.top() == rhs.p.top(); }
		bool operator !=(const iterator &rhs) const { return p.top() != rhs.p.top(); }
		ObjectHandle operator *() { return *p.top().first; }
		const ObjectHandle *operator ->() { return &*p.top().first; }
	};
	
	iterator begin() { return iterator(children.begin(), children.end()); }
	iterator end() { return iterator(children.end(), children.end()); }
	
	struct Constructor
	{
		typedef ObjectHandle (*Func)();
		typedef map<string,Func> List;
		
		static List list;
		
		Constructor(string name, Func func) { list[name] = func; }
	};
	
	static ObjectHandle construct(string);
	
	private:
	Point<double> parentOrigin;
	Quaternion<double> parentRotation;
};
//------------------------------------------------------------------------------
    
//! Represents a bounding box.
    
//! Point variables follow xyz where x is l(left) or r(ight), y is t(op) or b(ottom) and z is l(ow) or h(igh).
struct BoundingBox
{
    Point<double> lbl, rbl, ltl, rtl, lbh, rbh, lth, rth;
        
    BoundingBox(const BoundingBox &B)
    : lbl(B.lbl), rbl(B.rbl), ltl(B.ltl), rtl(B.rtl),
    lbh(B.lbh), rbh(B.rbh), lth(B.lth), rth(B.rth) {}
        
    BoundingBox(Point<double> _lbl, Point<double> _rbl, Point<double> _ltl, Point<double> _rtl = Point<double>(),
                Point<double> _lbh = Point<double>(), Point<double> _rbh = Point<double>(), Point<double> _lth = Point<double>(), Point<double> _rth = Point<double>())
                : lbl(_lbl), rbl(_rbl), ltl(_ltl), rtl(_rtl),
                    lbh(_lbh), rbh(_rbh), lth(_lth), rth(_rth) {}
        
    BoundingBox(Point<double> _lbl = Point<double>(), Point<double> _rth = Point<double>())
		: lbl(_lbl), rbl(0), ltl(0), rtl(0),
        lbh(0), rbh(0), lth(0), rth(_rth) {}
};
    
//------------------------------------------------------------------------------
    


} // namespace Core

#endif // _CORE_H

//------------------------------------------------------------------------------
