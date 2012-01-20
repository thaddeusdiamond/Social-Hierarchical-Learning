// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Just trying to get acquainted with Kinect

#include "Utils.h"
#include "MSR_NuiApi.h"
#include "KinectInterp.pb.h"

#include <MMSystem.h>
#include <csignal>

void GetDepth(KinectData* storage, HANDLE depth_stream_handle);
void GetVideo(KinectData* storage, HANDLE video_stream_handle);
void GetSkeleton(KinectData* storage);

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

  // Finally, we're going to be sending data so we open a socket
  SOCKET outgoing = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (outgoing == INVALID_SOCKET)
    DIE(-1, "Could not open a socket to talk on\n");
  
  // Initialize WinSock because it blows
  WSADATA wsaData;
  WORD version = MAKEWORD(1, 0);
  if (WSAStartup(version, &wsaData))
    DIE(-1, "Winsock blows!\n");

  // Bind the TCP connection
  SOCKADDR_IN target;
  target.sin_family = AF_INET;
  target.sin_port = htons(GLOB_PORT);
  target.sin_addr.s_addr = HOST_IP;
  if (connect(outgoing, (SOCKADDR *)&target, sizeof(target)) == SOCKET_ERROR)
    DIE(-1, "Could not connect to the desired server\n");

  fprintf(stdout, "Hit any key to begin sending primitive data...\n");
  fprintf(stdout, "Type 'q' to exit\nUse ctrl-c during to stop\n");
  while (getchar() != 'q') {
    Signal::ResetProgram();
    signal(SIGINT, &Signal::StopProgram);

    do {
      KinectData* storage = new KinectData();
      int next_event_id = WaitForMultipleObjects(
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
        // Create a proto to serialize (THIS SIDE OF WIRE)
        GetSkeleton(storage);
        break;

      case DEPT_EVT:
        GetDepth(storage, depth_stream_handle);
        total_frames++;
        break;
      }

      // Serialize and pass along the way using TCP
      if (storage->people_size() > 0) {
        string serialization;
        assert(storage->SerializeToString(&serialization));

        KinectData* data = new KinectData();
        assert(data->ParseFromString(serialization));
        send(outgoing, serialization.c_str(), storage->ByteSize(), 0);
      }

      // Get the current time and calculate FPS
      int current_time = timeGetTime();
      if (last_fps_time < 0)
        last_fps_time = current_time;

      // Only show output approximately once per second
      if (current_time - last_fps_time > 1000) {
        //fprintf(stdout, "Processing at %5.3f FPS\n",
        //        (1000 * total_frames) / static_cast<double>(current_time - last_fps_time));
        last_fps_time = current_time;
        total_frames = 0;
      }

    } while (Signal::ProgramIsRunning);

    fprintf(stdout, "\nSession Complete. Hit any key to begin sending primitive data...\n");
    fprintf(stdout, "Type 'q' to exit\nUse ctrl-c during to stop\n");
  }

  // Shut down the Kinect gracefully
  NuiShutdown();
  for (int i = 0; i < sizeof(nui_events) / sizeof(nui_events[0]); i++) {
    if (nui_events[i] && nui_events[i] != INVALID_HANDLE_VALUE)
      CloseHandle(nui_events[i]);
  }

  // Close the open sockets
  shutdown(outgoing, 2);
  closesocket(outgoing);
  WSACleanup();

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
void GetVideo(KinectData* storage, HANDLE video_stream_handle) {
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
void GetDepth(KinectData* storage, HANDLE depth_stream_handle) {
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
void GetSkeleton(KinectData* storage) {
  NUI_SKELETON_FRAME skeleton_frame;
  HRESULT hr = NuiSkeletonGetNextFrame(0, &skeleton_frame);
  if (FAILED(hr))
    DIE(hr, "Could not get skeleton frame from Kinect\n");
  
  // Smooth out the frame
  NuiTransformSmooth(&skeleton_frame, NULL);

  for (int i = 0; i < NUI_SKELETON_COUNT; i++) {
    // Create a representation of the person
    KinectData::Person* person = storage->add_people();
    person->set_id(i);

    // Skeleton is being tracked, get the <X, Y, Z> of that joint
    if (skeleton_frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
      person->set_active(true);
      for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; j++) {
        float x_pos, y_pos; USHORT z_pos;
        NuiTransformSkeletonToDepthImageF(skeleton_frame.SkeletonData[i].SkeletonPositions[j],
          &x_pos, &y_pos, &z_pos);

        KinectData::Person::SkeletonPosition* position = person->add_skeleton_positions();
        position->set_x(x_pos); position->set_y(y_pos); position->set_z(z_pos);
      }

    // Skeleton not being tracked 
    } else {
      person->set_active(false);
    }
  }
}