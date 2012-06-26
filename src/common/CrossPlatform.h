/*
 * Network module
 * 
 * Date: 26-04-12 12:29
 *
 * Description: Abstraction for internet communication
 *
 */

#ifndef _CROSSPLATFORM_H
#define _CROSSPLATFORM_H

#ifdef __APPLE__
	#define BACKSPACE ((char) 127)
	#define DEL '\b'
#else
	#define BACKSPACE '\b'
	#define DEL ((char) 127)
#endif

namespace CrossPlatform {
	void init();	
} // namespace CrossPlatform

#endif // _CROSSPLATFORM_H

//------------------------------------------------------------------------------
