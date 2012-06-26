/*
 * Lobby module -- see header file
 */
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif
#include <stdio.h>
namespace CrossPlatform {

void init(){
#if (defined WIN32 || defined _MSC_VER)

#else
    #ifdef __APPLE__
		CGSetLocalEventsSuppressionInterval(0.0);
	#else
		#ifdef __linux__ 
		//LINUX
		#else
		//unsupported
		printf("Warning: Platform might not be supported! \n");
		printf("Warning: The platforms: Windows NT, Mac OS X (>=10.7) and Linux are supported"\n);
		#endif
	#endif
#endif
}

} // namespace CrossPlatform

//------------------------------------------------------------------------------
