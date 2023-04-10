#pragma once
#include <Windows.h>
#include <condition_variable>

class ConsoleDisplay
{
public:
	int ScreenWidth;
	int ScreenHeight;
	CHAR_INFO* Screen;
	HANDLE hConsole;
	HANDLE hConsoleIn;
	SMALL_RECT RectWindow;
	std::wstring AppName;


	ConsoleDisplay()
	{
		ScreenWidth = 512;
		ScreenHeight = 128;

		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		AppName = L"FP Game";
	}

	void CreateConsole(int width, int height, int fontw, int fonth)
	{
		ScreenWidth = width;
		ScreenHeight = height;

		// Change console visual size to a minimum so ScreenBuffer can shrink
		// below the actual visual size
		RectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(hConsole, TRUE, &RectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
		SetConsoleScreenBufferSize(hConsole, coord);

		// Assign screen buffer to the console
		SetConsoleActiveScreenBuffer(hConsole);

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
		//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
		//else
		//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P


		wcscpy_s(cfi.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(hConsole, false, &cfi);

		// Get screen buffer info and check the maximum allowed window size. Return
		// error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(hConsole, &csbi);

		// Set Physical Console Window Size
		RectWindow = { 0, 0, (short)(ScreenWidth - 1), (short)(ScreenHeight - 1) };
		SetConsoleWindowInfo(hConsole, TRUE, &RectWindow);
		
		// Set flags to allow mouse input	
		SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		// Allocate memory for screen buffer
		Screen = new CHAR_INFO[ScreenWidth * ScreenHeight];
		memset(Screen, 0, sizeof(CHAR_INFO) * ScreenWidth * ScreenHeight);
	}

	void Start()
	{
		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (1)
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			float DeltaTime = elapsedTime.count();

			if (GetAsyncKeyState(VK_ESCAPE))
			{
				break;
			}

			OnUpdate(DeltaTime);

			// Update Title & Present Screen Buffer
			wchar_t s[256];
			swprintf_s(s, 256, L"%s - FPS: %3.2f", AppName.c_str(), 1.0f / DeltaTime);
			SetConsoleTitle(s);
			WriteConsoleOutput(hConsole, Screen, { (short)ScreenWidth, (short)ScreenHeight }, { 0,0 }, &RectWindow);
		}
	}

	virtual void OnUpdate(float DeltaTime){}
};