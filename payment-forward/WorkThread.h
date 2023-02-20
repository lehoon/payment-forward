#ifndef NOTIFY_FORWARD_NOTIFY_WORK_THREAD_H_
#define NOTIFY_FORWARD_NOTIFY_WORK_THREAD_H_


#include <windows.h>
#include "IThreadWrap.h"

class WorkThread : public IThreadWrap
{
public:
	WorkThread(IThreadUser* pWorker);
	virtual ~WorkThread();

	//thread interface
	virtual bool StartThread();
	virtual bool StopThread();

	static unsigned int WINAPI threadFunc(void* p);
	unsigned int DoWork();

private:
	IThreadUser* worker_;
};

#endif //NOTIFY_FORWARD_NOTIFY_WORK_THREAD_H_
