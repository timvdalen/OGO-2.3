/*******************************************************
 * image object -- header file                         *
 *                                                     *
 * Author: Ferry Timmers                               *
 *                                                     *
 * Date: 21-3-2012 17:06                               *
 *                                                     *
 * Description: Internal image representation          *
 *******************************************************/

#ifndef _IMAGE_H
#define _IMAGE_H

#include "base.h"

namespace Loader {

//------------------------------------------------------------------------------

//! Internal representation of an image
struct Image
{
	udword width;  //!< Image width in pixels
	udword height; //!< Image height in pixels
	ubyte  depth;  //!< Image color depth in bytes
	ubyte  *data;  //!< Image data \note allocated and freed automatically!
	
	//! Constructs a blank image file
	Image(udword width, udword height, ubyte depth);
	//! Destroys the image
	~Image();
	
	//! Image size in bytes
	udword size();
	
	protected:
	Image() : width(0), height(0), depth(0), data(0) {}
};

//------------------------------------------------------------------------------

//! Image from PNG file
struct PngImage : public Image
{
	//! Creates an image from a png file
	PngImage(const char *filename);
};

//------------------------------------------------------------------------------

} // namespace Loader

#endif /* _IMAGE_H */

//..............................................................................
