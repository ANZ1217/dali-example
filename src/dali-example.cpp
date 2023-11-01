/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// EXTERNAL INCLUDES
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/adaptor-framework/offscreen-application.h>
#include <dali/devel-api/adaptor-framework/offscreen-window.h>
#include <dali-toolkit/public-api/image-loader/image-url.h>
#include <dali-toolkit/public-api/image-loader/image.h>
#include <dali/public-api/adaptor-framework/encoded-image-buffer.h>
#include <dali/public-api/dali-adaptor-common.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/object/base-object.h>

#include <thread>

#include <Ecore.h>
#include <Eldbus.h>
#include <tbm_surface.h>
#include <tbm_surface_internal.h>
#include <tbm_surface_queue.h>

#include <sys/eventfd.h>
#include <unistd.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
  const char *const STYLE_PATH(DEMO_STYLE_DIR "dali-example.json"); ///< The style used for this example
  const char *const BACKGROUND_STYLE_NAME("Background");            ///< The name of the Background style
  const char *const IMAGE_STYLE_NAME("StyledImage");                ///< The name of the styled image style
  const char *const IMAGE_PATH(DEMO_IMAGE_DIR "silhouette.jpg");    ///< Image to show

  const int IMAGE_WIDTH = 256;
  const int IMAGE_HEIGHT = 256;

  std::thread gOffscreenThread;
  std::thread gMainInternalThread;
}

constexpr float APP_SECONDS_PER_FRAME          = 0.033f; ///< 30.0fps
constexpr float APP_TOTAL_RUNNING_TIME_SECONDS = 30.0f;
// unnamed namespace
class Example : public ConnectionTracker
{
public:
  Example(OffscreenApplication &app)
  : mApplication(app)
  {
    mApplication.InitSignal().Connect(this, &Example::Create);
    fprintf(stderr, "OffscreenExample Create\n");
  }

  void Create()
  {
    fprintf(stderr, "OffscreenExample::Create()\n");
    OffscreenWindow window = mApplication.GetWindow();

    ImageView image = ImageView::New(IMAGE_PATH);
    image.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    window.Add(image);

    mTimer = Timer::New(3000); // ms
    mTimer.TickSignal().Connect(this, &Example::OnTick);
    mTimer.Start();
  }

  bool OnTick()
  {
    fprintf(stderr, "OffscreenExample Quit\n");
    mApplication.Quit();
    return false;
  }

  OffscreenApplication &mApplication;
  Timer mTimer;
};

static void makeOffscreenApplication()
{
  tbm_surface_queue_h queue = tbm_surface_queue_create(3, 256, 256, TBM_FORMAT_ARGB8888, TBM_BO_DEFAULT);
  OffscreenApplication offscreenApplication = OffscreenApplication::New(queue, OffscreenApplication::RenderMode::AUTO);
  Example example(offscreenApplication);

  fprintf(stderr, "OffscreenExample MainLoop\n");
  offscreenApplication.MainLoop();
}

int gQuitFileDescriptor;
int gTimerFileDescriptor;

Eina_Bool AppTickTimer(void* data)
{
  fprintf(stderr, "AppTickTimer\n");

  uint64_t buffer = 1;
  fprintf(stderr, "timer fd[%d] write!\n", gTimerFileDescriptor);
  size_t size = write(gTimerFileDescriptor, &buffer, sizeof(uint64_t));
  if(size != sizeof(uint64_t))
  {
    fprintf(stderr, "timer fd[%d] write error!\n", gTimerFileDescriptor);
  }

  return EINA_TRUE;
}
Eina_Bool AppTerminateTimer(void* data)
{
  fprintf(stderr, "AppTerminateTimer\n");

  uint64_t buffer = 1;
  fprintf(stderr, "quit fd[%d] write!\n", gQuitFileDescriptor);
  size_t size = write(gQuitFileDescriptor, &buffer, sizeof(uint64_t));
  if(size != sizeof(uint64_t))
  {
    fprintf(stderr, "quit fd[%d] write error!\n", gQuitFileDescriptor);
  }

  gOffscreenThread.join();
  ecore_main_loop_quit();
  fprintf(stderr, "ecore_main_loop_quit\n");

  return EINA_FALSE;
}

static void main_internal()
{
  gQuitFileDescriptor = eventfd(0, EFD_NONBLOCK);
  gTimerFileDescriptor = eventfd(0, EFD_NONBLOCK);

  ecore_init();

  ecore_timer_add(APP_SECONDS_PER_FRAME, &AppTickTimer, nullptr);
  ecore_timer_add(APP_TOTAL_RUNNING_TIME_SECONDS, &AppTerminateTimer, nullptr);

  fprintf(stderr, "begin ecore_main_loop\n");
  ecore_main_loop_begin();
  fprintf(stderr, "end ecore_main_loop\n");
}

int DALI_EXPORT_API
main(int argc, char **argv)
{
  fprintf(stderr, "Main\n");

  gMainInternalThread = std::thread(main_internal);

  sleep(1);

  fprintf(stderr, "Thread 1 Start");

  gOffscreenThread = std::thread(makeOffscreenApplication);
  gOffscreenThread.join();

  fprintf(stderr, "Thread 1 Stop");

  sleep(1);

  fprintf(stderr, "Thread 2 Start");

  gOffscreenThread = std::thread(makeOffscreenApplication);
  gOffscreenThread.join();

  fprintf(stderr, "Thread 2 Stop");

  return 0;
}
