#include <OffscreenExample.h>

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>

// for TBM surface
#include <tbm_surface.h>
#include <tbm_surface_internal.h>
#include <tbm_surface_queue.h>

#include <chrono>
#include <thread>

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{
const char * const STYLE_PATH( DEMO_STYLE_DIR "dali-example.json" ); ///< The style used for this example
const char * const BACKGROUND_STYLE_NAME( "Background" ); ///< The name of the Background style
const char * const IMAGE_STYLE_NAME( "StyledImage" ); ///< The name of the styled image style
const char * const IMAGE_PATH( DEMO_IMAGE_DIR "silhouette.jpg" ); ///< Image to show
const char * const OFFSCREEN_IMAGE_PATH( DEMO_IMAGE_DIR "offscreen.jpg" ); ///< Image to show
} // unnamed namespace

OffscreenExample::OffscreenExample(OffscreenApplication& application)
: mApplication( application )
{
    fprintf(stderr, "OffscreenExample\n");
    mApplication.InitSignal().Connect( this, &OffscreenExample::Create );
}
OffscreenExample::~OffscreenExample()
{
    fprintf(stderr, "~OffscreenExample\n");
}

void OffscreenExample::Create()
{
    fprintf(stderr, "OffscreenExample::Create()\n");

    OffscreenWindow window = mApplication.GetWindow();

    ImageView image = ImageView::New( OFFSCREEN_IMAGE_PATH );
    image.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    window.Add( image );

    Timer timer = Timer::New(33); // ms
    timer.TickSignal().Connect(this, &OffscreenExample::OnTick);

    timer.Start();

    fprintf(stderr, "OffscreenExample::OnFrameRendered Create\n");
    window.SetFrameRenderedCallback(MakeCallback(this, &OffscreenExample::OnFrameRendered));
}

bool OffscreenExample::OnTick()
{
    return true; // Repeat unlimited.
}

void OffscreenExample::OnFrameRendered()
{
    OffscreenWindow window = mApplication.GetWindow();

    fprintf(stderr, "Offscreen OnFrameRendered Called\n");
    tbm_surface_queue_h queue = AnyCast<tbm_surface_queue_h>(window.GetNativeHandle());

    fprintf(stderr, "Offscreen DumpTbmToFile\n");
    //DumpTbmToFile(static_cast<void*>(queue));
    DumpTbmToBuffer(static_cast<void*>(queue));
}

void OffscreenExample::Terminate()
{
    fprintf(stderr, "OffscreenExample::Terminate()\n");
}