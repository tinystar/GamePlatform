#include "TrayIcon.h"
#include <tchar.h>


CTrayIcon::CTrayIcon(UINT uID)
{
	::memset(&m_TrayIcon, 0, sizeof(m_TrayIcon));
	m_TrayIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_TrayIcon.uID = uID;
}

CTrayIcon::~CTrayIcon()
{

}

void CTrayIcon::Init(HWND hWnd, UINT uMsg)
{
	m_TrayIcon.hWnd = hWnd;
	m_TrayIcon.uFlags = NIF_MESSAGE;
	m_TrayIcon.uCallbackMessage = uMsg;	
}

void CTrayIcon::ShowIcon(HICON hIcon, const TCHAR* pszTip)
{
	if (NULL == hIcon)
		return;

	m_TrayIcon.uFlags |= NIF_ICON;
	m_TrayIcon.hIcon = hIcon;
	if (pszTip != NULL && *pszTip != 0)
	{
		if (_tcslen(pszTip) < _countof(m_TrayIcon.szTip))
		{
			m_TrayIcon.uFlags |= NIF_TIP;
			_tcscpy(m_TrayIcon.szTip, pszTip);
		}
	}

	Shell_NotifyIcon(NIM_ADD, &m_TrayIcon);
}

BOOL CTrayIcon::ShowBalloon(const TCHAR* pInfoTitle, const TCHAR* pInfo)
{
	if (NULL == pInfo || 0 == *pInfo)
		return FALSE;
	if (_tcslen(pInfo) >= _countof(m_TrayIcon.szInfo))
		return FALSE;

	m_TrayIcon.uFlags |= NIF_INFO;
	m_TrayIcon.uTimeout = 1500;
	m_TrayIcon.dwInfoFlags = NIIF_INFO;
	_tcscpy(m_TrayIcon.szInfo, pInfo);
	if (pInfoTitle != NULL && *pInfoTitle != 0 && _tcslen(pInfoTitle) < _countof(m_TrayIcon.szInfoTitle))
		_tcscpy(m_TrayIcon.szInfoTitle, pInfoTitle);

	return Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
}

void CTrayIcon::DeleteIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
}
