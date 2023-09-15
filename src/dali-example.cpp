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

#include <tbm_surface.h>
#include <tbm_surface_internal.h>
#include <tbm_surface_queue.h>

// INTERNAL INCLUDES
#include <OffscreenExample.h>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const STYLE_PATH( DEMO_STYLE_DIR "dali-example.json" ); ///< The style used for this example
const char * const BACKGROUND_STYLE_NAME( "Background" ); ///< The name of the Background style
const char * const IMAGE_STYLE_NAME( "StyledImage" ); ///< The name of the styled image style
const char * const IMAGE_PATH( DEMO_IMAGE_DIR "silhouette.jpg" ); ///< Image to show

const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 256;
} // unnamed namespace

namespace
{
  std::thread gOffscreenThread;

  bool gOffscreenRendered = false;
  PixelData gOffscreenBuffer;
}

void DumpTbmToBuffer(void* data)
{
  static int index = 0;

  fprintf(stderr, "Offscreen DumpTbmToBuffer data : %p, index : %d\n", data, index);

  tbm_surface_queue_h queue = static_cast<tbm_surface_queue_h>(data);

  tbm_surface_h consumeSurface = NULL;

  // If the rendering to a surface is finished, the surface enqueue. Then you can acquire it.
  if(tbm_surface_queue_can_acquire(queue, 1))
  {
    if(tbm_surface_queue_acquire(queue, &consumeSurface) != TBM_SURFACE_QUEUE_ERROR_NONE)
    {
      fprintf(stderr, "Failed to acquire a tbm_surface\n");
      return;
    }
  }
  else
  {
    fprintf(stderr, "can_acquire() failed\n");
    return;
  }

  if(!consumeSurface)
  {
    fprintf(stderr, "consumeSurface is NULL\n");
    return;
  }
  index++;

  {
    tbm_surface_internal_ref(consumeSurface);

    int         dumpResult = 0;
    std::string filename   = "dali-example-" + std::to_string(index);

    // convert tbm_surface_h to Dali:Vector<uint8_t>
    tbm_surface_info_s info;
    dumpResult = tbm_surface_get_info(consumeSurface, &info);

    if(dumpResult == 0)
    {
      fprintf(stderr, "Offscreen tbm surface get info Success.\n");
    }
    else
    {
      fprintf(stderr, "Offscreen tbm surface get info failed.\n");
    }

    uint32_t bufferSize = info.planes[0].size;
    fprintf(stderr, "Offscreen bufferSize: %d\n", bufferSize);

    uint8_t* tmpBuffer = new uint8_t[bufferSize];
    for(uint32_t i=0;i<bufferSize;i++)
    {
      //convert ARGB to RGBA
      //tmpBuffer[i/4*4+(i+3)%4] = info.planes[0].ptr[i];
      tmpBuffer[i] = info.planes[0].ptr[i];
    }

    gOffscreenBuffer = PixelData::New(tmpBuffer, bufferSize, IMAGE_WIDTH, IMAGE_HEIGHT, Pixel::BGRA8888, PixelData::ReleaseFunction::DELETE_ARRAY);

    tbm_surface_internal_unref(consumeSurface);
  }

  if(tbm_surface_internal_is_valid(consumeSurface))
  {
    tbm_surface_queue_release(queue, consumeSurface);
  }

  consumeSurface = NULL;

  gOffscreenRendered = true;
}

// NOTE: Set valid path to get dump files
#define DUMP_PATH "/media/USBDriveA1"

void DumpTbmToFile(void* data)
{
  static int index = 0;

  fprintf(stderr, "DumpTbmToFile data : %p, index : %d\n", data, index);

  tbm_surface_queue_h queue = static_cast<tbm_surface_queue_h>(data);

  tbm_surface_h consumeSurface = NULL;

  // If the rendering to a surface is finished, the surface enqueue. Then you can acquire it.
  if(tbm_surface_queue_can_acquire(queue, 1))
  {
    if(tbm_surface_queue_acquire(queue, &consumeSurface) != TBM_SURFACE_QUEUE_ERROR_NONE)
    {
      fprintf(stderr, "Failed to acquire a tbm_surface\n");
      return;
    }
  }
  else
  {
    fprintf(stderr, "can_acquire() failed\n");
    return;
  }

  if(!consumeSurface)
  {
    fprintf(stderr, "consumeSurface is NULL\n");
    return;
  }
  index++;

  {
    tbm_surface_internal_ref(consumeSurface);

    int         dumpResult = 0;
    std::string filename   = "dali-example-" + std::to_string(index);

    // You can check the result of rendering by capturing the buffer.
    dumpResult = tbm_surface_internal_capture_buffer(consumeSurface, DUMP_PATH, filename.c_str(), "png");

    if(dumpResult == 0)
    {
      fprintf(stderr, "[DUMP] Failed on dump. Maybe file of same name exists? or folder not exists?\n");
    }
    else
    {
      fprintf(stderr, "[DUMP] The frame has been dumped in %s/%s.png\n", DUMP_PATH, filename.c_str());
    }

    tbm_surface_internal_unref(consumeSurface);
  }

  if(tbm_surface_internal_is_valid(consumeSurface))
  {
    tbm_surface_queue_release(queue, consumeSurface);
  }
  consumeSurface = NULL;
}

/// Basic DALi Example to use for debugging small programs on target
class Example : public ConnectionTracker
{
public:

  ///< Constructor
  Example( Application& application )
  : mApplication( application )
  {
    mApplication.InitSignal().Connect( this, &Example::Create );
  }

  ~Example() = default;

private:

  ///< Called to initialise the application UI

  static void makeOffscreenApplication()
  {
    tbm_surface_queue_h queue = tbm_surface_queue_create(3, 256, 256, TBM_FORMAT_ARGB8888, TBM_BO_DEFAULT);

    OffscreenApplication offscreenApplication = OffscreenApplication::New(queue, OffscreenApplication::RenderMode::AUTO);
    OffscreenExample offscreenTest( offscreenApplication );

    fprintf(stderr, "OffscreenExample MainLoop\n");
    offscreenApplication.MainLoop();
  }

  void Create( Application& application )
  {
    // Get a handle to the main window & respond to key events
    Window window = application.GetWindow();
    window.KeyEventSignal().Connect( this, &Example::OnKeyEvent );

    // Create the background using the style sheet
    Control control = Control::New();
    control.SetStyleName( BACKGROUND_STYLE_NAME );
    window.Add( control );

    // Create an ImageView and add it to the window
    ImageView image = ImageView::New( IMAGE_PATH );
    image.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    image.SetProperty( Actor::Property::PARENT_ORIGIN, Vector3( 0.5f,  0.25f, 0.5f ) );
    window.Add( image );

    // Create an ImageView with properties set from the style sheet
    ImageView styledImage = ImageView::New();
    styledImage.SetStyleName( IMAGE_STYLE_NAME );
    window.Add( styledImage );

    mTimer = Timer::New(33); // ms
    mTimer.TickSignal().Connect(this, &Example::OnTick);
    mTimer.Start();

    // offscreen application new

    fprintf(stderr, "OffscreenExample Create\n");
    gOffscreenThread = std::thread(makeOffscreenApplication);
  }

  ///< Called when a key is pressed, we'll use this to quit
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.GetState() == KeyEvent::DOWN )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  bool OnTick()
  {
    //fprintf(stderr, "Offscreen OnTick");

    if(gOffscreenRendered)
    {
      fprintf(stderr, "Offscreen gOffscreenRendered is True");
      Window window = mApplication.GetWindow();

    fprintf(stderr, "Offscreen GetImageUrl");
      ImageUrl mImageUrl = Image::GenerateUrl(gOffscreenBuffer);
      ImageView image = ImageView::New();

      Property::Map imagePropertyMap;
      imagePropertyMap.Insert(Visual::Property::TYPE, Visual::IMAGE);
      imagePropertyMap.Insert(ImageVisual::Property::URL, mImageUrl.GetUrl());

      image.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
      image.SetProperty( Actor::Property::PARENT_ORIGIN, Vector3( 0.5f,  0.5f, 0.5f ) );
      image.SetProperty( ImageView::Property::IMAGE, imagePropertyMap );

      fprintf(stderr, "Window Added New Image");
      window.Add( image );

      gOffscreenRendered = false;
    }

    return true;
  }

private:
  Application&  mApplication;
  Timer mTimer;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  fprintf(stderr, "Main\n");

  Application application = Application::New( &argc, &argv, STYLE_PATH );
  Example test( application );
  application.MainLoop();
  return 0;
}
