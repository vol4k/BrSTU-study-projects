// LAB03.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "LAB03.h"
#include <random>
#include <vector>

// Глобальные переменные:
HWND dlg;										// экземпляр окна
int dCount = 0;									// счетчик уже упавших шаров
int tCount = 0;									// число шариков (потоков)
int dead = 0;									// номер последнего упавшего

HANDLE tDlg;									// дин. массив потоков
std::vector<int> X, Y;							// дин. массив координат

// Отправить объявления функций, включенных в этот модуль кода:
INT_PTR CALLBACK    WND(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    GET_INFO(HWND, UINT, WPARAM, LPARAM);
DWORD	WINAPI		timer(LPVOID t);
DWORD	WINAPI		thread(LPVOID t);
DWORD	WINAPI		dInit(LPVOID t);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом 

	DialogBox(NULL, MAKEINTRESOURCE(IDD_GET_INFO), NULL, GET_INFO);

	if (tCount <= 0) return FALSE;

	X.resize(tCount);
	Y.resize(tCount);

	for (int i = 0; i < tCount; i++)
		X[i] = mersenne() % 400 + 25, Y[i] = -35;

	tDlg = CreateThread(NULL, NULL, dInit, NULL, NULL, NULL);
	CreateThread(NULL, NULL, timer, NULL, NULL, NULL);

	for (int i = 0; i < tCount; i++) {
		CreateThread(NULL, NULL, thread, (LPVOID)i, NULL, NULL);
	}
	WaitForSingleObject(tDlg, INFINITE);

	return FALSE;
}

INT_PTR CALLBACK WND(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND listbox;
	dlg = hDlg;
	UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
		listbox = GetDlgItem(hDlg, IDC_LIST);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
		if (LOWORD(wParam) == ID_CLOSE){
			EndDialog(hDlg, LOWORD(wParam));
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
		}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);

		Rectangle(hdc, 10, 0, 460, 250);
		
		if (dCount) {
			wchar_t str[50];
			wsprintf(str, L"%d-й шарик лопнул >_<", dead);
			SetDlgItemText(hDlg, ID_TEXT, str);
			volatile static int tDead = 0;
			if (tDead != dead) {
				SendMessage(listbox, LB_ADDSTRING, 0, (LPARAM)str);
				tDead = dead;
				if (dCount == tCount) {
					wsprintf(str, L"Все шарики лопнули T.T");
					SetDlgItemText(hDlg, ID_TEXT, str);
				}
			}
		}
		for(int i = 0; i < tCount;i++)
			Ellipse(hdc, X[i] + 0, Y[i] + 0, X[i] + 25, Y[i] + 35);

		EndPaint(hDlg, &ps);
	}
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK GET_INFO(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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

DWORD WINAPI CALLBACK timer(LPVOID t) {
	while (dCount != tCount) {
		Sleep(30);
		InvalidateRect(dlg, NULL, FALSE);
	}
	return 0;
}

DWORD WINAPI dInit(LPVOID t) {
	DialogBox(NULL, MAKEINTRESOURCE(IDD_WND), NULL, WND);
	return 0;
}

DWORD WINAPI thread(LPVOID t){
	int i = (int)t;

	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом 

	Sleep(mersenne() % (500*tCount));

	while (Y[i] < 215) {
		Sleep(10);
		if (mersenne() % 2) Y[i]++;
		else if (mersenne() % 2) X[i] += 3;
		else X[i] -= 3;
		if (X[i] < 10) X[i] = 430;
		else if (X[i] > 430) X[i] = 10;
	}
	Y[i] = -35;

	dCount++;

	dead = i + 1;

	return 0;
}