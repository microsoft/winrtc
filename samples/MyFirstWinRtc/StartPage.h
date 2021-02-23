#pragma once

#include "StartPage.g.h"

namespace winrt::MyFirstWinRtc::implementation
{
    struct StartPage : StartPageT<StartPage>
    {
        StartPage();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        join_room_Click(winrt::Windows::Foundation::IInspectable const &sender,
                        winrt::Windows::UI::Xaml::RoutedEventArgs const &e);

        void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
        void join_room_Click(winrt::Windows::Foundation::IInspectable const &sender,
                             winrt::Windows::UI::Xaml::RoutedEventArgs const &e);
    };
    }

namespace winrt::MyFirstWinRtc::factory_implementation
{
    struct StartPage : StartPageT<StartPage, implementation::StartPage>
    {
    };
}
