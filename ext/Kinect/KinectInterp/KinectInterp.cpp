// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Just trying to get acquainted with Kinect

#include "Utils.h"
#include "MSR_NuiApi.h"
#include "KinectInterp.pb.h"

#include <MMSystem.h>
#include <csignal>

void GetDepth(KinectInterpProto* storage, HANDLE depth_stream_handle);
void GetVideo(KinectInterpProto* storage, HANDLE video_stream_handle);
void GetSkeleton(KinectInterpProto* storage, pair<int, bool>* active_skeletons);

int main(int argc, char* argv[]) {
  // Set up the handles to be used in kinect NUI tracking
  HANDLE nui_events[4];
  nui_events[STOP_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[VIDE_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[DEPT_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);
  nui_events[SKEL_EVT] = CreateEvent(NULL, TRUE, FALSE, NULL);

  // Initialize and enable the Kinect and make sure it exists
  HRESULT hr;
  hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
                     NUI_INITIALIZE_FLAG_USES_SKELETON |
                     NUI_INITIALIZE_FLAG_USES_COLOR);
  if (FAILED(hr))
    DIE(hr, "Uh oh, couldn't find your Kinect device\n");

  // Enable skeleton tracking
  hr = NuiSkeletonTrackingEnable(nui_events[SKEL_EVT], 0);
  if (FAILED(hr))
    DIE(hr, "Could not enable skeletal tracking on the system\n");

  // Get the image stream from the Kinect
  HANDLE video_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
                          0, 2, nui_events[VIDE_EVT], &video_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect image stream\n");

  // Get the depth stream from the Kinect
  HANDLE depth_stream_handle;
  hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
                          NUI_IMAGE_RESOLUTION_320x240, 0, 2, 
                          nui_events[DEPT_EVT], &depth_stream_handle);
  if (FAILED(hr))
    DIE(hr, "Could not open Kinect depth stream\n");
  
  // We want to keep track of the FPS in a time var and handle SIGINT
  int last_fps_time = -1,
      total_frames = 0;
  signal(SIGINT, &Signal::StopProgram);

  // Globally modified loop variables (I know, bad comment...)
  int next_event_id = -1;
  pair<int, bool> active_skeletons[NUI_SKELETON_COUNT];
  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    active_skeletons[i].first = 0;
    active_skeletons[i].second = false;
  }

  do {
    // Create a proto to serialize (THIS SIDE OF WIRE)
    KinectInterpProto* storage = new KinectInterpProto();
    next_event_id = WaitForMultipleObjects(
      sizeof(nui_events) / sizeof(nui_events[STOP_EVT]),
      nui_events,  // Array containing the events we're waiting on
      FALSE,       // We don't want to wait on all events, just one
      100);        // Max wait of 100ms

    switch (next_event_id) {
    case STOP_EVT:
      Signal::StopProgram(0);
      break;
    
    case VIDE_EVT:
      GetVideo(storage, video_stream_handle);
      break;

    case SKEL_EVT:
      GetSkeleton(storage, active_skeletons);
      break;

    case DEPT_EVT:
      GetDepth(storage, depth_stream_handle);
      total_frames++;
      break;
    }

    // Serialize and pass along the way
    for (int i = 0; i < NUI_SKELETON_COUNT; i++)
      storage->add_active_skeletons(active_skeletons[i].second);
    string serialization;
    assert(storage->SerializeToString(&serialization));

    // TODO(Thad): Pass on wire

    // Create a proto to deserialize after rest (OTHER SIDE OF WIRE)
    KinectInterpProto* decompressor = new KinectInterpProto();
    assert(decompressor->ParseFromString(serialization));

    // Get the current time and calculate FPS
    int current_time = timeGetTime();
    if (last_fps_time < 0)
      last_fps_time = current_time;

    // Only show output approximately once per second
    if (current_time - last_fps_time > 1000) {
      fprintf(stdout, "The following skeleton IDs are being tracked: ");
      for (int i = 0; i < decompressor->active_skeletons_size(); i++)
        decompressor->active_skeletons(i) ? fprintf(stdout, "%d ", i) : NULL;
      
      fprintf(stdout, "\nProcessing at %5.3f FPS\n", (1000 * total_frames) /
              static_cast<double>(current_time - last_fps_time));
      last_fps_time = current_time;
      total_frames = 0;
    }

  } while (Signal::ProgramIsRunning);

  // Shut down the Kinect gracefully
  NuiShutdown();
  for (int i = 0; i < sizeof(nui_events) / sizeof(nui_events[0]); i++) {
    if (nui_events[i] && nui_events[i] != INVALID_HANDLE_VALUE)
      CloseHandle(nui_events[i]);
  }

  // Let the user see the CMD and then exit
  fprintf(stderr, "Press any key to exit...\n");
  getchar();
}

/**
 * Clear out the video back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the image data
 *   @param video_stream_handle Handle representing the video stream of a
 *                              Kinect device
 */
void GetVideo(KinectInterpProto* storage, HANDLE video_stream_handle) {
  const NUI_IMAGE_FRAME* image_frame_ptr = NULL;

  HRESULT hr = NuiImageStreamGetNextFrame(video_stream_handle, 0,
                                          &image_frame_ptr);
  if (FAILED(hr))
    DIE(hr, "Could not get image frame from Kinect\n");

  NuiImageStreamReleaseFrame(video_stream_handle, image_frame_ptr);
}

/**
 * Clear out the depth back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the depth data
 *   @param depth_stream_handle Handle representing the depth stream of a
 *                              Kinect device
 */
void GetDepth(KinectInterpProto* storage, HANDLE depth_stream_handle) {
  const NUI_IMAGE_FRAME* image_frame_ptr = NULL;

  HRESULT hr = NuiImageStreamGetNextFrame(depth_stream_handle, 0,
                                          &image_frame_ptr);
  if (FAILED(hr))
    DIE(hr, "Could not get depth frame from Kinect\n");

  NuiImageStreamReleaseFrame(depth_stream_handle, image_frame_ptr);
}

/**
 * Clear out the skeleton back buffer so we can keep receiving events
 *
 *   @param storage             A protocol buffer for storing the skeleton data
 */
void GetSkeleton(KinectInterpProto* storage, pair<int, bool>* active_skeletons) {
  NUI_SKELETON_FRAME skeleton_frame;
  
  HRESULT hr = NuiSkeletonGetNextFrame(0, &skeleton_frame);
  if (FAILED(hr))
    DIE(hr, "Could not get skeleton frame from Kinect\n");

  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    // This skeleton is active, let's track it
    if (skeleton_frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
      active_skeletons[i].first = timeGetTime();

      // Alert the user that we found a new skeleton
      if (!active_skeletons[i].second) {
        active_skeletons[i].second = true;
        fprintf(stdout, "Welcome, newbie! I'm gonna call you #%d!\n", i);
      }

    // Consider a skeleton out of frame if we haven't seen them for 0.5sec
    } else if (active_skeletons[i].first &&
               active_skeletons[i].first + 500 < timeGetTime()) {
      fprintf(stdout, "Bye #%d, it was good to have you around!\n", i);
      active_skeletons[i].first = 0;
      active_skeletons[i].second = false;
    }
  }
}