#include "pch.h"
#include "webrtc.VideoFrame.Builder.h"
#include "webrtc.VideoFrame.Builder.g.cpp"

#include "webrtc.VideoFrame.VideoFrame.h"

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::implementation
{
    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::VideoFrame Builder::Build()
    {
      throw 10;
      //return make<winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::implementation::VideoFrame>(
      //  webrtc_video_frame_builder_.build());
    }

    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder Builder::SetTimestampMs(int64_t timestamp_ms)
    {
      webrtc_video_frame_builder_.set_timestamp_ms(timestamp_ms);
      return *get_abi(this);
    }

    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder Builder::SetTimestampUs(int64_t timestamp_us)
    {
      webrtc_video_frame_builder_.set_timestamp_us(timestamp_us);
      return *get_abi(this);
    }

    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder Builder::SetTimestampRtp(uint32_t timestamp_rtp)
    {
      webrtc_video_frame_builder_.set_timestamp_rtp(timestamp_rtp);
      return *get_abi(this);
    }

    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder Builder::SetNtpTimeMs(int64_t ntp_time_ms)
    {
      webrtc_video_frame_builder_.set_ntp_time_ms(ntp_time_ms);
      return *get_abi(this);
    }

    Microsoft::WinRTC::WebRtcWrapper::webrtc::VideoFrame::Builder Builder::SetId(uint16_t id)
    {
      webrtc_video_frame_builder_.set_id(id);
      return *get_abi(this);
    }
}
