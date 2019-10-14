#ifndef SDL_RENDERER_H_
#define SDL_RENDERER_H_

#include <memory>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include <SDL.h>

#include "api/media_stream_interface.h"
#include "api/scoped_refptr.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "rtc_base/critical_section.h"

#include "rtc/video_track_reciever.h"

class SDLRenderer : public VideoTrackReciever {
 public:
  SDLRenderer();
  ~SDLRenderer();

  void SetIOContext(boost::asio::io_context *ioc);

  static int RenderThreadExec(void *data);
  int RenderThread();

  void SetOutlines();
  void AddTrack(webrtc::VideoTrackInterface* track) override;
  void RemoveTrack(webrtc::VideoTrackInterface* track) override;

 protected:
  class Sink : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
   public:
    Sink(SDLRenderer* renderer, webrtc::VideoTrackInterface* track);
    ~Sink();

    void OnFrame(const webrtc::VideoFrame& frame) override;

    void SetOutlineRect(int x, int y, int width, int height);

    rtc::CriticalSection* GetCriticalSection();
    bool GetOutlineChanged();
    int GetOffsetX();
    int GetOffsetY();
    int GetWidth();
    int GetHeight();
    uint8_t* GetImage();

   private:
    SDLRenderer* renderer_;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track_;
    rtc::CriticalSection frame_params_lock_;
    int outline_offset_x_;
    int outline_offset_y_;
    int outline_width_;
    int outline_height_;
    bool outline_changed_;
    float outline_aspect_;
    int input_width_;
    int input_height_;
    std::unique_ptr<uint8_t[]> image_;
    int offset_x_;
    int offset_y_;
    int width_;
    int height_;
  };

 private:
  void PollEvent();

  rtc::CriticalSection sinks_lock_;
  typedef std::vector<std::pair<webrtc::VideoTrackInterface*, std::unique_ptr<Sink> > >
      VideoTrackSinkVector;
  VideoTrackSinkVector sinks_;
  bool running_;
  boost::asio::io_context *ioc_;
  SDL_Thread *thread_;
  SDL_Window* window_;
  SDL_Renderer *renderer_;
  int width_;
  int height_;
  int rows_;
  int cols_;
  float window_aspect_;
  bool is_wide_;
};

#endif