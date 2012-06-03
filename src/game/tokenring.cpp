/*
 * Network algorithms -- see header file for more information
 */

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#if (defined WIN32 || defined _MSC_VER)
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <stdint.h>
#elif (defined __MACH__)
	#include <mach/clock.h>
	#include <mach/mach.h> 
#endif

//#include <set>
//#include <map>
//#include <queue>

#include "netalg.h"

#if (defined WIN32 || defined _MSC_VER)
	void getthetime(timespec *tsp)
	{
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		int64_t t = (*(int64_t *) &ft - ( ((int64_t) 27111902UL << 32) + (int64_t) 3577643008UL ));
		tsp->tv_sec  = (int) (t / 10000000);
		tsp->tv_nsec = (int) ((t - ((int64_t) tsp->tv_sec * (int64_t) 10000000)) * 100);
	}
#elif (defined __MACH__)
	void getthetime(timespec *tsp)
	{
		clock_serv_t cclock;
		mach_timespec_t mts;
		host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
		clock_get_time(cclock, &mts);
		mach_port_deallocate(mach_task_self(), cclock);
		tsp->tv_sec = mts.tv_sec;
		tsp->tv_nsec = mts.tv_nsec;
	}
#else
	#define getthetime(tsp) clock_gettime(CLOCK_REALTIME, tsp)
#endif

#define VPRIV(T,x) if (!data) return; T *x = (T *) data;
#define PRIV(T,x) if (!data) return false; T *x = (T *) data;

#define PRO\
BE {printf(__FILE__ ":%d\n", __LINE__);fflush(stdout);}

namespace Protocol {
	
//#define DECIDED (qd->connection != CliqueData::cnPending)
//#define INCOMMING (!qd->entry.empty() || !qd->loss.empty() || !qd->msgs.empty())
//#define CONNECTED (qd->connection == CliqueData::cnOpen)

using namespace std;
using namespace Net;

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

} // namespace Protocol

//------------------------------------------------------------------------------
