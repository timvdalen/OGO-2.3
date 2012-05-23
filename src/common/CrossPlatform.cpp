/*
 * Lobby module -- see header file
 */

#ifdef __APPLE__
	#include "CoreFoundation/CoreFoundation.h"
#endif


namespace CrossPlatform {

void init(){
#if (defined WIN32 || defined _MSC_VER)
	init_win32();
#else
	#ifdef __APPLE__
		init_apple();
	#else
		#ifdef __linux__ 
			init_linux();
		#else
			init_unsupported();
		#endif
#endif
}
void init_linux(){
	
}
void init_apple(){
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
    if(DEBUG){
	std::cout << "Current Path: " << path << std::endl; // error: expected constructor, destructor or type conversion before '<<' token
    }
}

void init_win32(){
	
}

void init_unsupported(){
	printf("Warning: Platform might not be supported! \n");
	printf("Warning: The platforms: Windows NT, Mac OS X (>=10.7) and Linux are supported"\n);
}

} // namespace CrossPlatform

//------------------------------------------------------------------------------
