#ifndef SIMPLE_EVENT_H
#define SIMPLE_EVENT_H

#ifdef _MSC_VER
#include <WinSock2.h>
#include <Windows.h>
#endif //_MSC_VER

#ifdef __GNUC__
#include <pthread.h>
#endif //__GNUC__

/*
 * @shilei. TODO: This is an important thing, the event class need to be modified for a perfect running resuls.
 *          An event class based on condition variable is a bad way.
 * 2017.01.06
 */

class SimpleEvent
{

public:
	SimpleEvent();
	~SimpleEvent();

	void SignalEvent();
	void WaitEvent();
	//void setHandleProc();


private:	
#ifdef __GNUC__
	pthread_cond_t cond;
	pthread_mutex_t mutex;
    bool bIsSignal;
#endif // __GNUC__
	
#ifdef _MSC_VER
	HANDLE cond;
#endif //_MSC_VER

};

#endif //SIMPLE_EVENT_H
