// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// clang-format off
#include "pch.h"
#include "webrtc.AudioTrackSink.h"
#include "webrtc.AudioTrackSink.g.cpp"
// clang-format on

#include "winrt/Windows.Storage.Streams.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;

struct __declspec(uuid("5b0d3235-4dba-4d44-865e-8f1d0e4fd04d")) __declspec(novtable) IMemoryBufferByteAccess
    : ::IUnknown
{
  virtual HRESULT __stdcall GetBuffer(uint8_t **value, uint32_t *capacity) = 0;
};

struct __declspec(uuid("905a0fef-bc53-11df-8c49-001e4fc686da")) IBufferByteAccess : ::IUnknown
{
  virtual HRESULT __stdcall Buffer(uint8_t **value) noexcept = 0;
};

struct BufferView : implements<BufferView, IBuffer, IBufferByteAccess, IClosable>
{
  const uint8_t *data_;
  size_t capacity_;

  BufferView(const uint8_t *data, size_t capacity) : data_(data), capacity_(capacity)
  {
  }

  ~BufferView() noexcept
  {
  }

  void
  Close() noexcept
  {
  }

  uint32_t
  Capacity() const noexcept
  {
    return static_cast<uint32_t>(capacity_);
  }

  uint32_t
  Length() const noexcept
  {
    return static_cast<uint32_t>(capacity_);
  }

  void
  Length(uint32_t value)
  {
    UNREFERENCED_PARAMETER(value);
  }

  HRESULT __stdcall Buffer(uint8_t **value) noexcept final
  {
    *value = const_cast<uint8_t *>(data_);
    return S_OK;
  }
};

namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
{
winrt::event_token
AudioTrackSink::OnData(Microsoft::WinRTC::WebRtcWrapper::webrtc::AudioTrackSinkOnData const &handler)
{
  return on_audio_track_sink_on_data_event_.add(handler);
}

void
AudioTrackSink::OnData(winrt::event_token const &token) noexcept
{
  on_audio_track_sink_on_data_event_.remove(token);
}

void
AudioTrackSink::OnData(const void *audio_data, int bits_per_sample, int sample_rate, size_t number_of_channels,
                       size_t number_of_frames)
{
  // FIXME(auright): Check if this method works.
  size_t capacity = (bits_per_sample >> 3) * sample_rate * number_of_channels * number_of_frames;

  on_audio_track_sink_on_data_event_(make<BufferView>(static_cast<const uint8_t *>(audio_data), capacity),
                                     bits_per_sample, sample_rate, static_cast<uint32_t>(number_of_channels),
                                     static_cast<uint32_t>(number_of_frames));
}
} // namespace winrt::Microsoft::WinRTC::WebRtcWrapper::webrtc::implementation
