#pragma once
#include <Windows.h>
#include <condition_variable>

class ConsoleDisplay
{
public:
	int ScreenWidth;
	int ScreenHeight;
	CHAR_INFO* Screen;
	HANDLE ConsoleHandle;
	HANDLE ConsoleInHandle;
	SMALL_RECT RectWindow;
	std::wstring GameName;


	ConsoleDisplay()
	{
		ScreenWidth = 512;
		ScreenHeight = 128;

		ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		ConsoleInHandle = GetStdHandle(STD_INPUT_HANDLE);

		GameName = L"FP Game";
	}

	void CreateConsole(int width, int height, int fontw, int fonth)
	{
		ScreenWidth = width;
		ScreenHeight = height;

		// Set the size of the screen buffer
		COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
		SetConsoleScreenBufferSize(ConsoleHandle, coord);

		// Assign screen buffer to the console
		SetConsoleActiveScreenBuffer(ConsoleHandle);

		// Set the font size
		CONSOLE_FONT_INFOEX Font;
		Font.cbSize = sizeof(Font);
		Font.nFont = 0;
		Font.dwFontSize.X = fontw;
		Font.dwFontSize.Y = fonth;
		Font.FontFamily = FF_DONTCARE;
		Font.FontWeight = FW_NORMAL;

		// Assign font
		wcscpy_s(Font.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(ConsoleHandle, false, &Font);

		// Set Physical Console Window Size
		RectWindow = { 0, 0, (short)(ScreenWidth - 1), (short)(ScreenHeight - 1) };
		SetConsoleWindowInfo(ConsoleHandle, TRUE, &RectWindow);
		
		// Set flags to allow mouse input	
		SetConsoleMode(ConsoleInHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

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

			wchar_t s[256];
			swprintf_s(s, 256, L"%s - FPS: %3.2f", GameName.c_str(), 1.0f / DeltaTime);
			SetConsoleTitle(s);
			WriteConsoleOutput(ConsoleHandle, Screen, { (short)ScreenWidth, (short)ScreenHeight }, { 0,0 }, &RectWindow);
		}
	}

	virtual void OnUpdate(float DeltaTime){}
};