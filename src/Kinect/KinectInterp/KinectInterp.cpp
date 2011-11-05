// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Just trying to get acquainted with Kinect

#include <windows.h>
#include <csignal>
#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <iostream>
#include <string>

#include "MSR_NuiApi.h"
#include "KinectInterp.pb.h"

using std::string;

static inline void DIE(HRESULT hr, const char* format, ...) {
  va_list arguments;
  va_start(arguments, format);

  fprintf(stderr, format, arguments);
  fprintf(stderr, "  [Code - 0x%08X, Facility - 0x%08X, Severity - 0x%08X]\n",
          HRESULT_CODE(hr), HRESULT_FACILITY(hr), HRESULT_SEVERITY(hr));

  fprintf(stderr, "Press any key to exit...\n");
  getchar();
  exit(hr);
}

namespace Signal {
  bool ProgramIsRunning = true;

  static inline void StopProgram(int status) {
    ProgramIsRunning = false;
  }
}

int main(int argc, char* argv[]) {
  // TODO(Thad): Use kinect protos here to serialize data
  //KinectProto* proto = new KinectProto();
  //proto->set_id(100);

  // Initialize and enable the Kinect and make sure it exists
  HRESULT hr;
  hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
                     NUI_INITIALIZE_FLAG_USES_SKELETON |
                     NUI_INITIALIZE_FLAG_USES_COLOR);
  if (FAILED(hr))
    DIE(hr, "Uh oh, couldn't find your Kinect device\n");

  // Set up the handles to be used in kinect NUI tracking
  HANDLE next_skeleton_event, next_video_frame_event, next_depth_frame_event;
  next_depth_frame_event = CreateEvent(NULL, TRUE, FALSE, NULL);
  next_video_frame_event = CreateEvent(NULL, TRUE, FALSE, NULL);
  next_skeleton_event    = CreateEvent(NULL, TRUE, FALSE, NULL);

  // Enable skeleton tracking
  hr = NuiSkeletonTrackingEnable(next_skeleton_event, 0);
  if (FAILED(hr))
    DIE(hr, "Could not enable skeletal tracking on the system\n");

  // Get the image stream from the Kinect
  HANDLE video_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
                          0, 2, next_video_frame_event, &video_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect image stream\n");

  // Get the depth stream from the Kinect
  HANDLE depth_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
                          NUI_IMAGE_RESOLUTION_320x240, 0, 2, 
                          next_depth_frame_event, &depth_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect depth stream\n");
    
  // Main loop: Process data from the Kinect
  signal(SIGINT, &Signal::StopProgram);
  do {
    // TODO(Thad): Get frame information

    fprintf(stdout, "Spinning...\n");
    Sleep(1000);
  } while (Signal::ProgramIsRunning);

  // Shut down the Kinect gracefully
  NuiShutdown();
  if (next_skeleton_event && next_skeleton_event != INVALID_HANDLE_VALUE)
    CloseHandle(next_skeleton_event);
  if (next_video_frame_event && next_video_frame_event != INVALID_HANDLE_VALUE)
    CloseHandle(next_video_frame_event);
  if (next_depth_frame_event && next_depth_frame_event != INVALID_HANDLE_VALUE)
    CloseHandle(next_depth_frame_event);

  // Let the user see the CMD and then exit
  fprintf(stderr, "Press any key to exit...\n");
  getchar();
}