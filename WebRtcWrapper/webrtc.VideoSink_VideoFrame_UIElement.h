#pragma once
#include "webrtc.VideoSink_VideoFrame_UIElement.g.h"


namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
    struct VideoSink_VideoFrame_UIElement : VideoSink_VideoFrame_UIElementT<VideoSink_VideoFrame_UIElement>
    {
       /* VideoSink_VideoFrame_UIElement() = default;*/

        VideoSink_VideoFrame_UIElement(Windows::UI::Xaml::UIElement const& canvas);
        ~VideoSink_VideoFrame_UIElement();

        ::rtc::VideoSinkInterface<::webrtc::VideoFrame> *get_webrtc_video_sink_uielement_video_frame() const;

      private:
        void *webrtc_video_sink_uielement_video_frame_;
    };
}
namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::factory_implementation
{
    struct VideoSink_VideoFrame_UIElement : VideoSink_VideoFrame_UIElementT<VideoSink_VideoFrame_UIElement, implementation::VideoSink_VideoFrame_UIElement>
    {
    };
}
