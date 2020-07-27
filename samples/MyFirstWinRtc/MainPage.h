#pragma once

#include "MainPage.g.h"

namespace winrt::MyFirstWinRtc::implementation
{
	struct MainPage : MainPageT<MainPage>
	{
		MainPage();
	};
}

namespace winrt::MyFirstWinRtc::factory_implementation
{
	struct MainPage : MainPageT<MainPage, implementation::MainPage>
	{
	};
}
