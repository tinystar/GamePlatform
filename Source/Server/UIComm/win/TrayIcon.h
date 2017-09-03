#ifndef __TRAY_ICON_H__
#define __TRAY_ICON_H__

#include <wtypes.h>
#include <shellapi.h>

class CTrayIcon
{
public:
	explicit CTrayIcon(UINT uID);
	~CTrayIcon();

public:
	void Init(HWND hWnd, UINT uMsg);
	void ShowIcon(HICON hIcon, const TCHAR* pszTip);
	BOOL ShowBalloon(const TCHAR* pInfoTitle, const TCHAR* pInfo);
	void DeleteIcon();

private:
	NOTIFYICONDATA	m_TrayIcon;
};

#endif // __TRAY_ICON_H__
