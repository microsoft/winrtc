//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "../../Microsoft.WinRTC.Simple.VideoConferencing - Copy/Generated Files/AppRtcControl.xaml.g.h"
#include "../../Microsoft.WinRTC.Simple.VideoConferencing - Copy/Generated Files/AppRtcControl.xaml.g.hpp"

using namespace Signalig;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();

	auto room = to_hstring(CryptographicBuffer::GenerateRandomNumber());
	Control().Room(room);
}
