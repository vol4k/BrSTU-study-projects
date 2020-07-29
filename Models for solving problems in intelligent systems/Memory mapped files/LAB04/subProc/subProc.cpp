#include "framework.h"
#include "subProc.h"
#include <random>
#include <vector>

// Глобальные переменные:
int					index = -1;			// номер экземпляра
volatile bool		wait;				// маркер ожидания
int*				pvFile;				// указатель на буффер проекции
HWND				dlg;				// экземпляр диалогового окна

// Отправить объявления функций, включенных в этот модуль кода:
INT_PTR CALLBACK    STATE(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		InitDlg(LPVOID);
DWORD WINAPI		thread(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом

	///////////////////////////////////////////////////////////////////////////////////////////////

	HANDLE hFile = CreateFile(L"C:\\data.dat", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Файл не может быть открыт.", L"Ошибка", MB_OK);
		return(FALSE);
	}

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, 0, NULL);

	if (hFileMap == NULL) {
		MessageBox(NULL, L"Проекция файла не может быть открыта.", L"Ошибка", MB_OK);
		CloseHandle(hFile);
		return(FALSE);
	}

	pvFile = (int*)MapViewOfFileEx(hFileMap, FILE_MAP_WRITE, 0, 0, 0, nullptr);

	if (pvFile == NULL) {
		MessageBox(NULL, L"Не удалось отобразить вид файла.", L"Ошибка", MB_OK);
		CloseHandle(hFileMap);
		CloseHandle(hFile);
		return(FALSE);
	}

	for (int i = 1; i <= pvFile[0] * 3; i += 3)
		if (!pvFile[i + 2]) { index = i; break; }

	if (index != -1) {

		booked = 1;
		HANDLE init;

		init = CreateThread(NULL, NULL, InitDlg, NULL, NULL, NULL);
		CreateThread(NULL, NULL, thread, NULL, NULL, NULL);
		WaitForSingleObject(init, INFINITE);

		booked = 0;
	}
	else
		MessageBox(NULL, L"Превышено число одновременно открытых экземпляров", L"Ошибка", MB_OK);

	// Clean up everything before exiting.
	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return FALSE;
}


////////////////////////////////////////////////////////////////
DWORD WINAPI thread(LPVOID t) {
	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом 

	try {
		while (true) {
			wait = true;

			while (wait);

			while (pvFile[index + 1] < 215) {
				Sleep(10);
				if (mersenne() % 2) Y++;
				else if (mersenne() % 2) X += 3;
				else X -= 3;
				if (X < 10) X = 430;
				else if (X > 430) X = 10;
				InvalidateRect(dlg, NULL, FALSE);
			}
			Y = -35;
		}
	}
	catch (...) {}

	return 0;
}
////////////////////////////////////////////////////////////////

DWORD WINAPI InitDlg(LPVOID t) {
	DialogBox(NULL, MAKEINTRESOURCE(IDD_STATE), NULL, STATE);
	return TRUE;
}

INT_PTR CALLBACK STATE(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	dlg = hDlg;

	if (!pvFile[0]) EndDialog(hDlg, FALSE);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		wchar_t str[32];
		wsprintf(str, L"Состояние шарика #%d", index / 3 + 1);
		SetDlgItemText(hDlg, IDC_STATUS, str);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_PUSH)
			wait = false;
		break;
	case WM_CLOSE:
		EndDialog(hDlg, LOWORD(wParam));
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);

		wchar_t pos[5];
		wsprintf(pos, L"%d", X);
		SetDlgItemText(hDlg, IDC_X, pos);
		wsprintf(pos, L"%d", Y);
		SetDlgItemText(hDlg, IDC_Y, pos);

		EndPaint(hDlg, &ps);
		break;
	}
	return (INT_PTR)FALSE;
}

