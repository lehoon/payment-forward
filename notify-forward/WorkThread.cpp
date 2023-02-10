
#include "WorkThread.h"
#include <process.h>

WorkThread::WorkThread(IThreadUser* pWorker)
{
	threadHandle_ = NULL;
	threadId_ = 0;
	worker_ = pWorker;
}
WorkThread::~WorkThread()
{
	StopThread();
}

bool WorkThread::StartThread()
{
	//ASSERT_NOT_NULL(worker_);
	//LOG_DEBUG("beging start theard. first call worker's InitTask()");
	if (!worker_->InitTask()) {
		//LOG_DEBUG("InitTask failed.");
		return false;
	}

	threadHandle_ = (HANDLE)_beginthreadex(NULL, 0,
		WorkThread::threadFunc, (void*)this, 0, &threadId_);
	//LOG_DEBUG("thread start successfully, id is:" << threadId_);
	return threadHandle_ != 0;
}
bool WorkThread::StopThread()
{
	if (NULL != threadHandle_) {
		//LOG_DEBUG("stop thread... with id:" << threadId_);
		quitEt_.SetEvent();
		WaitForSingleObject(threadHandle_, 100);
		CloseHandle(threadHandle_);
		threadHandle_ = NULL;
		//LOG_DEBUG("stop success.");
	}
	//LOG_DEBUG("work thread stop successfully.");
	return TRUE;
}
unsigned int WorkThread::threadFunc(void* p)
{
	WorkThread* threadObj = (WorkThread*)p;
	return threadObj->DoWork();
}
unsigned int WorkThread::DoWork()
{
	HANDLE hEvts[2];
	hEvts[0] = quitEt_;
	hEvts[1] = suspendEt_;
	while (TRUE) {
		DWORD dwRet = WaitForMultipleObjects(2, hEvts, FALSE, 0);
		dwRet -= WAIT_OBJECT_0;
		if (dwRet == 0) {
			//LOG_INFO("got a quit single, so the thread exit.");
			break;
		}
		if (dwRet == 1) {
			suspendOKEt_.SetEvent();
			::SuspendThread(threadHandle_);
			continue;
		}

		if (worker_ != nullptr && !worker_->Work()) {
			//LOG_ERROR("work got a error, so the thread exit.");
			break;//quit thread
		}
	}

	worker_->ExitTask();

	_endthreadex(0);
	return 0;
}

