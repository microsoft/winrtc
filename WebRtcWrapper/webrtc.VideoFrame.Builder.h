#pragma once
#include "webrtc.VideoFrame.Builder.g.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::implementation
{
    struct Builder : BuilderT<Builder>
    {
        Builder() = default;

        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::VideoFrame Build();
        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder SetTimestampMs(int64_t timestamp_ms);
        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder SetTimestampUs(int64_t timestamp_us);
        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder SetTimestampRtp(uint32_t timestamp_rtp);
        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder SetNtpTimeMs(int64_t ntp_time_ms);
        Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder SetId(uint16_t id);

        ::webrtc::VideoFrame::Builder webrtc_video_frame_builder_;
    };
}
namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::factory_implementation
{
    struct Builder : BuilderT<Builder, implementation::Builder>
    {
    };
}
