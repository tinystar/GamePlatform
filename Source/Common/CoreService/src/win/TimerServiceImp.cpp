#include "TimerServiceImp.h"
#include <process.h>


TimerServiceImp::TimerServiceImp()
	: m_stateFlags(0)
	, m_hMsgWnd(NULL)
	, m_hThreadEvent(INVALID_HANDLE_VALUE)
{

}

TimerServiceImp::~TimerServiceImp()
{
	stop();
	unInit();
}

SVCErrorCode TimerServiceImp::init()
{
	if (m_stateFlags & kInited)
		return eOk;

	m_hThreadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!EzVerify(m_hThreadEvent != NULL))
		return eSystemError;

	m_stateFlags |= kInited;
	return eOk;
}

SVCErrorCode TimerServiceImp::unInit()
{
	if (!(m_stateFlags & kInited))
		return eOk;

	if (m_hThreadEvent != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hThreadEvent);
		m_hThreadEvent = INVALID_HANDLE_VALUE;
	}

	m_stateFlags &= ~kInited;
	return eOk;
}

SVCErrorCode TimerServiceImp::start()
{
	if (!(m_stateFlags & kInited))
		return eNotInitialized;
	if (m_stateFlags & kRunning)
		return eOk;

	HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, timerThreadProc, this, 0, NULL);
	if (!EzVerify(hThread != NULL))
	{
		EzLogError(_T("create timer thread fail!\n"));
		return eSystemError;
	}
	::WaitForSingleObject(m_hThreadEvent, INFINITE);
	::ResetEvent(m_hThreadEvent);
	::CloseHandle(hThread);

	if (!EzVerify(m_hMsgWnd != NULL))
		return eSystemError;

	m_stateFlags |= kRunning;
	return eOk;
}

SVCErrorCode TimerServiceImp::stop()
{
	if (!(m_stateFlags & kRunning))
		return eOk;

	EzAssert(m_hMsgWnd != NULL);
	::SendMessage(m_hMsgWnd, WM_CLOSE, 0, 0);
	m_hMsgWnd = NULL;
	::ResetEvent(m_hThreadEvent);

	m_stateFlags &= ~kRunning;
	return eOk;
}

bool TimerServiceImp::setTimer(EzUInt uTimerId, EzUInt uElapse)
{
	if (NULL == m_hMsgWnd)
		return false;

	UINT_PTR ret = ::SetTimer(m_hMsgWnd, uTimerId, uElapse, NULL);
	return ret != 0;
}

bool TimerServiceImp::killTimer(EzUInt uTimerId)
{
	if (NULL == m_hMsgWnd)
		return false;

	return !!::KillTimer(m_hMsgWnd, uTimerId);
}

void TimerServiceImp::dump(EzDumpContext& dc) const
{

}

unsigned __stdcall TimerServiceImp::timerThreadProc(void* pParam)
{
	TimerServiceImp* pTimerInst = (TimerServiceImp*)pParam;
	EzAssert(pTimerInst != NULL);

	HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);
	WNDCLASSEX wndClassEx;
	
	wndClassEx.cbSize = sizeof(wndClassEx);
	wndClassEx.style = 0;
	wndClassEx.lpfnWndProc = timerWndProc;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = hInst;
	wndClassEx.hIcon = NULL;
	wndClassEx.hCursor = NULL;
	wndClassEx.hbrBackground = NULL;
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = _T("TimerMsgWnd@XX");
	wndClassEx.hIconSm = NULL;

	::RegisterClassEx(&wndClassEx);

	HWND hMsgOnlyWnd = ::CreateWindow(_T("TimerMsgWnd@XX"), NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInst, pTimerInst);
	if (NULL == hMsgOnlyWnd)
	{
		::SetEvent(pTimerInst->m_hThreadEvent);
		return -1;
	}

	pTimerInst->m_hMsgWnd = hMsgOnlyWnd;
	::SetEvent(pTimerInst->m_hThreadEvent);

	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK TimerServiceImp::timerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_CREATE:
		{
			TimerServiceImp* pTimerSvc = (TimerServiceImp*)(((CREATESTRUCT*)lParam)->lpCreateParams);
			::SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pTimerSvc);
		}
		break;
	case WM_TIMER:
		{
			TimerServiceImp* pTimerSvc = (TimerServiceImp*)::GetWindowLongPtr(hWnd, GWL_USERDATA);
			if (EzVerify(pTimerSvc != NULL))
			{
				pTimerSvc->notifyTimerMsg((EzUInt)wParam);
			}
		}
		break;
	case WM_CLOSE:
		::DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return ::DefWindowProc(hWnd, uMessage, wParam, lParam);
	}

	return 0;
}

void TimerServiceImp::notifyTimerMsg(EzUInt uTimerId)
{
	for (int i = 0; i < m_EventHandlers.logicalLength(); ++i)
	{
		ITimerServiceEventHandler* pEventHandler = m_EventHandlers[i];
		if (pEventHandler)
			pEventHandler->onTimerMessage(uTimerId);
	}
}
