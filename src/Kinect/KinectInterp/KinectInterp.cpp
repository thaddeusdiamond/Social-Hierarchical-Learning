// Author: Thaddeus Diamond (diamond@cs.yale.edu)
//
// Just trying to get acquainted with Kinect

#include <windows.h>
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

int main(int argc, char* argv[]) {
  // TODO(Thad): Use kinect protos here to serialize data
  //KinectProto* proto = new KinectProto();
  //proto->set_id(100);

  // Initialize the Kinect and make sure it exists
  HRESULT hr = NuiInitialize(
                   NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX |
                   NUI_INITIALIZE_FLAG_USES_SKELETON |
                   NUI_INITIALIZE_FLAG_USES_COLOR);
  if (FAILED(hr))
    DIE(hr, "Uh oh, couldn't find your Kinect device\n");

  // TODO(Thad): Put in some real action here

  // Shut down the Kinect gracefully
  NuiShutdown();

  fprintf(stderr, "Press any key to exit...\n");
  getchar();
  exit(EXIT_SUCCESS);
}