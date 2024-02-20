#include <Windows.h>
#include "resource.h"
#include "tchar.h"
#include <time.h>
#include <algorithm>
#include <vector>
#include <CommCtrl.h> // UDM_SETBUDDY


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


const int NUMBER_OF_BUTTONS = 16;
int buttonIds[NUMBER_OF_BUTTONS];
std::vector<int> generatedNumbers = {};
std::vector<int> guessedNumbers = {};

INT_PTR CALLBACK DialogProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeButtons(HWND hwnd);
void StartNewGame(HWND hwnd);
void CheckButton(HWND hwnd, WPARAM wParam);
void UpdateTimer(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
    for (int i = 0; i < NUMBER_OF_BUTTONS; ++i) {
        buttonIds[i] = IDC_BUTTON1 + i;
    }

    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProcedure);
}

void InitializeButtons(HWND hwnd) {
    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
        EnableWindow(GetDlgItem(hwnd, buttonIds[i]), FALSE);
    }
}

void StartNewGame(HWND hwnd) {
    generatedNumbers.clear();

    int timerCount = GetDlgItemInt(hwnd, IDC_EDIT2, NULL, TRUE);

    for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
        EnableWindow(GetDlgItem(hwnd, buttonIds[i]), TRUE);
        generatedNumbers.push_back(10 + rand() % 90);
        SetDlgItemInt(hwnd, buttonIds[i], generatedNumbers[i], TRUE);
    }

    std::sort(generatedNumbers.begin(), generatedNumbers.end());

    SetWindowText(GetDlgItem(hwnd, IDC_LIST1), L"");
    SetTimer(hwnd, 1, 1000, NULL);
    SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_SETRANGE, 0, MAKELPARAM(0, 100));
    SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_SETPOS, 0, 0);


    SetDlgItemInt(hwnd, IDC_EDIT2, timerCount, TRUE);
}

void CheckButton(HWND hwnd, WPARAM wParam) {
    if (LOWORD(wParam) >= IDC_BUTTON1 && LOWORD(wParam) <= IDC_BUTTON16 && HIWORD(wParam) == BN_CLICKED) {
        HWND clickedButton = GetDlgItem(hwnd, LOWORD(wParam));
        TCHAR buttonText[250];
        GetWindowText(clickedButton, buttonText, sizeof(buttonText) / sizeof(TCHAR));
        int buttonValue = _ttoi(buttonText);

        if (buttonValue == generatedNumbers[guessedNumbers.size()]) {
            guessedNumbers.push_back(buttonValue);
            EnableWindow(clickedButton, FALSE);
            SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_ADDSTRING, 0, LPARAM(buttonText));
        }

        if (guessedNumbers.size() == NUMBER_OF_BUTTONS) {
            KillTimer(hwnd, 1);
            MessageBox(hwnd, L"You win!", L"INFO", MB_ICONINFORMATION);
        }
    }
}

void UpdateTimer(HWND hwnd) {
    int timerCount = GetDlgItemInt(hwnd, IDC_EDIT2, NULL, TRUE);
    timerCount--;

    if (timerCount + 1 == 0) {
        KillTimer(hwnd, 1);
        MessageBox(hwnd, L"You lose!", L"INFO", MB_ICONINFORMATION);
    }

    SetDlgItemInt(hwnd, IDC_EDIT2, timerCount, TRUE);
    SendMessage(GetDlgItem(hwnd, IDC_PROGRESS1), PBM_STEPIT, 0, 0);
}

INT_PTR CALLBACK DialogProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE: {
        EndDialog(hwnd, 0);
        return TRUE;
    }
    case WM_INITDIALOG: {
        SetWindowText(hwnd, L"GAME");
        InitializeButtons(hwnd);
        return TRUE;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_NEWGAME && HIWORD(wParam) == BN_CLICKED) {
            StartNewGame(hwnd);
        }
        else {
            CheckButton(hwnd, wParam);
        }
        return TRUE;
    }
    case WM_TIMER: {
        UpdateTimer(hwnd);
        return TRUE;
    }
    }

    return FALSE;
}
