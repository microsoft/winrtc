#pragma once
#include <unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

#include <winrt/Microsoft.Graphics.Canvas.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Composition.h>
#include <winrt/Microsoft.Graphics.Canvas.UI.Xaml.h>

#pragma warning(push)
#pragma warning(disable : 4100 4127)
#include "rtc_base/ref_count.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/thread.h"
#include "api/audio_options.h"
#include "api/jsep.h"
#include "api/peer_connection_interface.h"
#include "api/scoped_refptr.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/create_peerconnection_factory.h"
#include "api/peer_connection_interface.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video/video_source_interface.h"
#include "api/video/video_frame.h"
#include "api/rtp_transceiver_interface.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "pc/video_track_source.h"

#include "media/base/video_adapter.h"
#include "media/base/video_broadcaster.h"
#include "rtc_base/critical_section.h"
#include "api/scoped_refptr.h"
#include "api/video/i420_buffer.h"
#include "api/video/video_frame_buffer.h"
#include "api/video/video_rotation.h"

#include "libyuv/libyuv.h"

// mkdir include
// copy common_types.h include
// robocopy /mir api include\api *.h
// robocopy /mir rtc_base include\rtc_base *.h
// robocopy /mir pc include\pc *.h
// robocopy /mir media include\media *.h
// robocopy /mir system_wrappers\include include\system_wrappers\include *.h
// robocopy /mir call include\call *.h
// robocopy /mir modules include\modules *.h
// robocopy /mir common_video include\common_video *.h
// robocopy /mir p2p include\p2p *.h
// robocopy /mir logging include\logging *.h
// xcopy /S /Y third_party\abseil-cpp\absl\*.h include\absl\
// xcopy /S /Y third_party\libyuv\include\*.h include\libyuv\


// xcopy /s /y common_video\*.h \WinRTC_Build\WinRTC\packages\Microsoft.WinRTC.libwebrtc.80.0.7-alpha\build\native\include\common_video\


#pragma warning(pop)
