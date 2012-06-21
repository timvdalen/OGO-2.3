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

namespace Broadcaster {
	
	void Initialize();
	void Terminate();
	
	void* broadcaster(void* ignoreBitch);

}

#endif // _BROADCASTER_H

//------------------------------------------------------------------------------
