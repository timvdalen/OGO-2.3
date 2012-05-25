#ifndef _LOADER3DS_H
#define _LOADER3DS_H

//--------------------------------------------

#include "core.h"

namespace Loader {

//--------------------------------------------

class ModelObject public Object {
	udword i;
public:
	ModelObject(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>());
	virtual ~ModelObject() {};
	virtual void render();
}

//--------------------------------------------

} // namespace Loader

#endif _LOADER3DS_H