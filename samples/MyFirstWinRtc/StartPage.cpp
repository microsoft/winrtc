#include "pch.h"
#include "StartPage.h"
#if __has_include("StartPage.g.cpp")
#include "StartPage.g.cpp"
#endif
#include <Generated Files\App.xaml.g.h>
#include "../Microsoft.WinRTC.Simple.VideoConferencing/Generated Files/AppRtcControl.xaml.g.h"
#include "../Microsoft.WinRTC.Simple.VideoConferencing/Generated Files/winrt/Windows.UI.Xaml.Controls.h"
#include "../Microsoft.WinRTC.Simple.VideoConferencing/Generated Files/winrt/impl/Windows.UI.Xaml.Controls.2.h"
#include "../Microsoft.WinRTC.Simple.VideoConferencing/Generated Files/AppRtcControl.xaml.g.hpp"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml;    
using namespace Windows::UI::Xaml::Controls;    
using namespace Windows::UI::Xaml::Controls::Primitives;    
using namespace Windows::UI::Xaml::Data;    
using namespace Windows::UI::Xaml::Input;    
using namespace Windows::UI::Xaml::Media;    
using namespace Windows::UI::Xaml::Navigation;    
using namespace Windows::UI::Xaml::Interop;

namespace winrt::MyFirstWinRtc::implementation
{
    StartPage::StartPage()
    {
        InitializeComponent();
    }

    int32_t StartPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void StartPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void winrt::MyFirstWinRtc::implementation::StartPage::join_room_Click(winrt::Windows::Foundation::IInspectable const &sender,
                                                                 winrt::Windows::UI::Xaml::RoutedEventArgs const &e)
    {
        this.join
    }
} // namespace winrt::MyFirstWinRtc::implementation



