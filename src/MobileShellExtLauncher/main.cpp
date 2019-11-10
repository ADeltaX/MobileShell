#include "pch.h"
#include <stdio.h>
#include <string>
#include <Shlwapi.h>
#include <vector>
#pragma comment(lib, "shlwapi.lib")

typedef WINBASEAPI BOOL(WINAPI* CreateProcessHid)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
);

void InjectDll(HANDLE hProcess, HANDLE hThread, const std::wstring& path)
{
	size_t strSize = (path.size() + 1) * sizeof(WCHAR);
	LPVOID pBuf = VirtualAllocEx(hProcess, 0, strSize, MEM_COMMIT, PAGE_READWRITE);
	if (pBuf == NULL)
		return;

	SIZE_T written;
	if (!WriteProcessMemory(hProcess, pBuf, path.c_str(), strSize, &written))
		return;

	LPVOID pLoadLibraryW = GetProcAddress(GetModuleHandle(L"kernel32"), "LoadLibraryW");
	QueueUserAPC((PAPCFUNC)pLoadLibraryW, hThread, (ULONG_PTR)pBuf);
}

std::wstring GetExecutableDir()
{
	WCHAR buf[MAX_PATH];
	GetModuleFileName(nullptr, buf, MAX_PATH);
	PathRemoveFileSpec(buf);
	return buf;
}

typedef BOOL(WINAPI* SetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	std::wstring path = GetExecutableDir();
	path += L"\\MobileShellExtension.dll";

	STARTUPINFO startInfo = { 0 };
	PROCESS_INFORMATION procInfo = { 0 };
	WCHAR cmdline[] = L"MiniBroker.exe";

	startInfo.cb = sizeof(startInfo);

	const auto createprocess = CreateProcessHid(GetProcAddressNew(L"kernel32.dll", L"CreateProcessW"));

	if (createprocess(nullptr, cmdline, nullptr, nullptr, FALSE, CREATE_SUSPENDED, nullptr, nullptr, &startInfo, &procInfo))
	{
		InjectDll(procInfo.hProcess, procInfo.hThread, path);
		ResumeThread(procInfo.hThread);
	}

	return 0;
}