#pragma once
#include <Windows.h>
#include <string>

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

	ConsoleDisplay(int witdh, int height, int fontSize, std::wstring gameName)
	{
		ScreenWidth = witdh;
		ScreenHeight = height;

		GameName = gameName;
		
		ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		ConsoleInHandle = GetStdHandle(STD_INPUT_HANDLE);

		SetScreenBuffer();

		SetFont(fontSize);

		// Set Physical Console Window Size
		RectWindow = { 0, 0, (short)(ScreenWidth - 1), (short)(ScreenHeight - 1) };
		SetConsoleWindowInfo(ConsoleHandle, TRUE, &RectWindow);

		// Set flags to allow mouse input	
		SetConsoleMode(ConsoleInHandle, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

		// Allocate memory for screen buffer
		Screen = new CHAR_INFO[ScreenWidth * ScreenHeight];
		memset(Screen, 0, sizeof(CHAR_INFO) * ScreenWidth * ScreenHeight);
	}

private:
	void SetScreenBuffer()
	{
		// Set the size of the screen buffer
		COORD coord = { (short)ScreenWidth, (short)ScreenHeight };
		SetConsoleScreenBufferSize(ConsoleHandle, coord);

		// Assign screen buffer to the console
		SetConsoleActiveScreenBuffer(ConsoleHandle);
	}

	void SetFont(int fontSize)
	{
		// Set the font size
		CONSOLE_FONT_INFOEX Font;
		Font.cbSize = sizeof(Font);
		Font.nFont = 0;
		Font.dwFontSize.X = fontSize;
		Font.dwFontSize.Y = fontSize;
		Font.FontFamily = FF_DONTCARE;
		Font.FontWeight = FW_NORMAL;

		// Assign font
		wcscpy_s(Font.FaceName, L"Consolas");
		SetCurrentConsoleFontEx(ConsoleHandle, false, &Font);
	}

public:
	void DrawConsole(float fps)
	{
		wchar_t title[256];
		swprintf_s(title, 256, L"%s - FPS: %.f", GameName.c_str(), fps);
		SetConsoleTitle(title);
		
		WriteConsoleOutput(ConsoleHandle, Screen, { (short)ScreenWidth, (short)ScreenHeight }, { 0,0 }, &RectWindow);
	}
};

