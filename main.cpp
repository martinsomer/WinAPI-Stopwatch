#include <windows.h>
#include <iomanip>

#define ID_TIMER 10
#define ID_START_PAUSE 20
#define ID_RESET 21

HWND hStartPauseButton;
HWND hResetButton;
UINT_PTR uTimer;

DWORD dSecondsElapsed = 0;
BOOL bStopwatchRunning = false;
LPCTSTR bStartPauseButtonText[] = {"Start", "Stop"};

void toggleButtonState() {
	bStopwatchRunning = EnableWindow(hResetButton, !bStopwatchRunning);
	SetWindowText(hStartPauseButton, bStopwatchRunning? bStartPauseButtonText[1] : bStartPauseButtonText[0]);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_CREATE: {
			uTimer = SetTimer(hwnd, ID_TIMER, 1000, (TIMERPROC) NULL);
			
			hStartPauseButton = CreateWindow("BUTTON", bStartPauseButtonText[0],
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                33, 83, 
                75, 25,
                hwnd,
				(HMENU) ID_START_PAUSE,
                NULL, NULL);
			
			hResetButton = CreateWindow("BUTTON", "Reset",
                WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_DISABLED,
                142, 83, 
                75, 25,
                hwnd,
				(HMENU) ID_RESET,
                NULL, NULL);
	  
	    	break;
		}
		
		case WM_TIMER: {
			if (bStopwatchRunning) {
				dSecondsElapsed++;
				RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
			}
			
			break;
		}
		
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case ID_START_PAUSE: {
					toggleButtonState();
					break;
				}
				case ID_RESET: {
					dSecondsElapsed = 0;
					toggleButtonState();
					RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
				}
			}
			
			break;
		}
		
		case WM_PAINT: {
			// Background for buttons
			PAINTSTRUCT ps;
			RECT rect;
    		HDC hdc = BeginPaint(hwnd, &ps);
//    		GetClientRect(hwnd, &rect);
			
			rect.top = 75;
			rect.bottom = 150;
			rect.left = 0;
			rect.right = 250;
			
			FillRect(hdc, &rect, (HBRUSH) GetSysColorBrush(COLOR_MENU));
			
			// Stopwatch text
			HFONT hFont = CreateFont(45, 0, 0, 0,
				FW_LIGHT,
				FALSE, FALSE, FALSE,
				DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_MODERN,
				"Arial");
				
			SelectObject(hdc, hFont);
			SetTextAlign(hdc, TA_CENTER);
			
			WORD wHours = dSecondsElapsed / 3600;
			WORD wMinutes = (dSecondsElapsed / 60) % 60;
			WORD wSeconds = dSecondsElapsed % 60;
			
			std::stringstream sTimeStream;
			sTimeStream << std::setfill('0') << std::setw(2) << wHours << ':' << std::setw(2) << wMinutes << ':' << std::setw(2) << wSeconds;
			const char* cTimeString = sTimeStream.str().c_str();
			
			TextOut(hdc, 125, 17, cTimeString, strlen(cTimeString));
			
	        break;
	    }
		
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	memset(&wc, 0, sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);

	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); // Load a standard icon
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); // use the name "A" to use the project icon

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
		"WindowClass",
		"Stopwatch",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
		250, 150, // Width, height
		NULL, // Parent window
		NULL, // No menu
		hInstance,
		NULL); // No pointer

	if (hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}
