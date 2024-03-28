#include "Main.h"
#include "../Console/Console.h"
#include <chrono>


ConsoleDisplay* Console = new ConsoleDisplay(200, 100, 8, L"FP Game");

int main()
{
	
}

void Start()
{
	using clock = std::chrono::high_resolution_clock;

	auto timeNow = clock::now();
	auto timePrevious = timeNow;

	while (1)
	{
		timeNow = clock::now();
		std::chrono::duration<float> elapsedTime = timeNow - timePrevious;
		timePrevious = timeNow;
		float DeltaTime = elapsedTime.count();

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			break;
		}

		EventTick(DeltaTime);

		Console->DrawConsole(1.0f / DeltaTime);
	}
}

void EventTick(float DeltaTime)
{

}