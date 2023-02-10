#ifndef NOTIFY_FORWARD_NOTIFY_BASE_EVENT_H_
#define NOTIFY_FORWARD_NOTIFY_BASE_EVENT_H_

#include <windows.h>

namespace base {

    class Event
    {
    public:
        explicit Event(BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE,
            LPCTSTR lpszNAme = NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL);

    public:
        BOOL SetEvent();
        BOOL PulseEvent();
        BOOL ResetEvent();

    public:
        virtual ~Event();
        operator HANDLE() const { return m_hObject; }
        HANDLE  m_hObject;
    };

}

#endif //NOTIFY_FORWARD_NOTIFY_BASE_EVENT_H_


