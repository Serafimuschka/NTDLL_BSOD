#include <Windows.h>
#include <winnt.h>
#include <winternl.h>
#include <ntstatus.h>
#pragma comment(lib, "ntdll.lib")
#define RTL_PRIVILEGE_SHUTDOWN 19

typedef NTSTATUS(NTAPI* ntdll_RtlAdjustPrivilege)(ULONG privilege, BOOLEAN isEnabled,
	BOOLEAN isCurrentThread, PBOOLEAN wasEnabled);

typedef NTSTATUS(NTAPI* ntdll_NtRaiseHardError)(NTSTATUS errorStatus, ULONG numberOfParameters,
	ULONG unicodeStringParameterMask OPTIONAL, PULONG_PTR parameters, ULONG responseOption,
	PULONG response);

LRESULT CALLBACK wndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND window;
	MSG msg;
	WNDCLASS wClass;

	wClass.style = CS_HREDRAW | CS_VREDRAW;
	wClass.lpfnWndProc = wndProc;
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;
	wClass.hInstance = hInstance;
	wClass.hIcon = NULL;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = "NTAPI Sample Application";

	if (!RegisterClass(&wClass))
	{
		MessageBox(NULL, "Can't register window class", "Error", MB_OK);
		return 0;
	}

	window = CreateWindow(
		wClass.lpszClassName,
		"NtRaiseHardError: call BSOD",
		WS_OVERLAPPEDWINDOW,
		50, 50,
		320, 240,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!window)
	{
		MessageBox(NULL, "Can't create main window", "Error", MB_OK);
		return 0;
	}
	else {
		MessageBox(NULL, "Your system will crash after clicking OK", "Warning!", MB_OK | MB_ICONWARNING);
	}

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	LPVOID ntdllRtlAdjustPrivelegeAddress =
		GetProcAddress(LoadLibrary("ntdll.dll"), "RtlAdjustPrivilege");
	if (!ntdllRtlAdjustPrivelegeAddress) {
		MessageBox(NULL, "Loading 'ntdll.dll' failed", "Error", MB_OK);
	}

	BOOLEAN bEnabled;
	ntdll_RtlAdjustPrivilege ntdllCallRtlAdjustPrivilege =
		(ntdll_RtlAdjustPrivilege)ntdllRtlAdjustPrivelegeAddress;
	NTSTATUS RtlAdjustPrivilege =
		ntdllCallRtlAdjustPrivilege(RTL_PRIVILEGE_SHUTDOWN, TRUE, FALSE, &bEnabled);

	void* ntdllNtRaiseHardErrorAddress =
		GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError");
	if (!ntdllNtRaiseHardErrorAddress) {
		MessageBox(NULL, "Handle ntdll.dll failed", "Error", MB_OK);
	}

	ULONG response;
	ntdll_NtRaiseHardError ntdllNtRaiseHardError =
		(ntdll_NtRaiseHardError)ntdllNtRaiseHardErrorAddress;
	NTSTATUS NtRaiseHardError =
		ntdllNtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, 0, 6, &response);

	if (!NtRaiseHardError) {
		MessageBox(NULL, "Force BSOD failed", "Error", MB_OK);
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK wndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_PAINT:
		
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(window, msg, wParam, lParam));
	}
}