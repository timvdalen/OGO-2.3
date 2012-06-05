/*
 * Lobby module -- see header file
 */
#ifdef __APPLE__
#ifdef __i386__
    #include "CoreFoundation/CoreFoundation.h"
#endif
#endif
#include <stdio.h>
namespace CrossPlatform {

void init(){
#if (defined WIN32 || defined _MSC_VER)

#else
    #ifdef __APPLE__
    #ifdef __i386__
    // ----------------------------------------------------------------------------
    // This makes relative paths work in C++ on MAC OSX by changing directory to the Resources folder inside the .app bundle  
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) // Error: expected unqualified-id before 'if'
    	{
        	// error!
		printf("Error: expected unqualified-id");
	}
    	CFRelease(resourcesURL); // error: expected constructor, destructor or type conversion before '(' token
    	chdir(path); // error: expected constructor, destructor or type conversion before '(' token
    	#ifdef DEBUG
		std::cout << "Current Path: " << path << std::endl; // error: expected constructor, destructor or type conversion before '<<' token
        #endif
        #endif
	#else
		#ifdef __linux__ 
		//LINUX
		#else
		//unsupported
		printf("Warning: Platform might not be supported! \n");
		printf("Warning: The platforms: Windows NT, Mac OS X (>=10.7) and Linux are supported"\n);
		#endif
	#endif
#endif*/
}

} // namespace CrossPlatform

//------------------------------------------------------------------------------
