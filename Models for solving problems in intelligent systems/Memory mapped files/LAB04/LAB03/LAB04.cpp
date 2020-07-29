// LAB03.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "LAB04.h"
#include <random>
#include <vector>

// Глобальные переменные:
int					tCount = 0;			// число шариков (процессов)
int					dead = 0;			// номер последнего упавшего
int					size;				// размер проекции
int*				pvFile;				// указатель на буффер проекции
HWND				dlg;				// экземпляр диалогового окна

// Отправить объявления функций, включенных в этот модуль кода:
INT_PTR CALLBACK    WND(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    GET_INFO(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI		dInit(LPVOID);
DWORD WINAPI		timer(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом 

	if (copieCount) {
		MessageBox(NULL, L"Копия программы уже запущена!", L"Ошибка", MB_OK);
		return FALSE;
	}
	copieCount++;

	DialogBox(NULL, MAKEINTRESOURCE(IDD_GET_INFO), NULL, GET_INFO);

	if (tCount <= 0) return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////////

	size = sizeof(int) * tCount * 3 + 1;

	HANDLE hFile = CreateFile(L"C:\\data.dat", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Файл не может быть открыт.", L"Ошибка", MB_OK);
		return(FALSE);
	}

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE,
		0, size, NULL);

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

	pvFile[0] = tCount;

	///////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 1; i <= tCount * 3; i += 3)
		pvFile[i] = mersenne() % 400 + 25, pvFile[i + 1] = -35, pvFile[i+2] = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////

	HANDLE init = CreateThread(NULL, NULL, dInit, NULL, NULL, NULL);
	CreateThread(NULL, NULL, timer, NULL, NULL,NULL);

	WaitForSingleObject(init, INFINITE);

	pvFile[0] = 0;

	// Clean up everything before exiting.
	UnmapViewOfFile(pvFile);
	CloseHandle(hFileMap);
	SetEndOfFile(hFile);
	CloseHandle(hFile);

	return FALSE;

}

DWORD WINAPI CALLBACK timer(LPVOID t) {
	while (true) {
		Sleep(30);
		InvalidateRect(dlg, NULL, FALSE);
	}
	return 0;
}

DWORD WINAPI dInit(LPVOID t) {
	DialogBox(NULL, MAKEINTRESOURCE(IDD_WND), NULL, WND);
	return 0;
}

INT_PTR CALLBACK WND(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	dlg = hDlg;

	static HWND listbox;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
		{
		case WM_INITDIALOG:
			listbox = GetDlgItem(hDlg, IDC_LIST);
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == ID_CLOSE) {
				EndDialog(hDlg, LOWORD(wParam));
				PostQuitMessage(0);
				return (INT_PTR)TRUE;
			}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hDlg, &ps);

			Rectangle(hdc, 10, 0, 460, 250);

			if (dead) {
				wchar_t str[50];
				wsprintf(str, L"%d-й шарик лопнул >_<", dead);
				SetDlgItemText(hDlg, ID_TEXT, str);
				volatile static int tDead = 0;
				if (tDead != dead) {
					SendMessage(listbox, LB_ADDSTRING, 0, (LPARAM)str);
					tDead = dead;
				}
			}

			for (int i = 1; i <= tCount * 3; i += 3) {
				Ellipse(hdc, pvFile[i] + 0, pvFile[i + 1] + 0, pvFile[i] + 25, pvFile[i + 1] + 35);
				if (pvFile[i + 1] >= 210) dead = i / 3 + 1;
			}

			EndPaint(hDlg, &ps);
		}
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK GET_INFO(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_OK) {
			wchar_t InputString[5];
			GetDlgItemText(hDlg, IDC_EDIT, InputString, 4);
			tCount = wcstol(InputString, 0, 10);
			EndDialog(hDlg, LOWORD(wParam));
		}
		else if (LOWORD(wParam) == ID_CANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}