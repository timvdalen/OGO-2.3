/*
 * World object
 *
 * Date: 08-06-12 16:00
 *
 * Description: Object that defines the root object of the game
 *
 */

#ifndef _BROADCASTER_H
#define _BROADCASTER_H
#include <pthread.h>
#include "../common/net.h"

namespace Broadcaster {
using namespace Net;
	
	struct Thread {
		bool run;
		pthread_t broadcastThread;
		UDPSocket udp;
	};
	extern Thread t;
	
	void Initialize();
	void Terminate();
	
	void* broadcaster(void* ignoreBitch);

}

#endif // _BROADCASTER_H

//------------------------------------------------------------------------------
