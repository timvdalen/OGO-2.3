#ifndef _LOADER3DS_H
#define _LOADER3DS_H

//------------------------------------------------------------------------------

#include "core.h"
#include "base.h"

#include <lib3ds.h>
#include <string>

//------------------------------------------------------------------------------

namespace Loader {

using namespace Base;
using namespace Core;

//------------------------------------------------------------------------------

class ModelObject : public Object {
	Lib3dsFile * file;
	void load(const char * path);
	void displaylist(Lib3dsMesh * mesh);
	void render(Lib3dsNode * n);
public:
	ModelObject(Point<double> P = Point<double>(), Quaternion<double> R = Quaternion<double>(), string path = "");
	virtual ~ModelObject();
	virtual void draw();
};

//------------------------------------------------------------------------------

} // namespace Loader

#endif // _LOADER3DS_H

//------------------------------------------------------------------------------
