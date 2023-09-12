#include <common/common.h>
#include <memory> ///< for std::unique_ptr

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

// TODO: 이미지 분리
    ImageView image = ImageView::New( IMAGE_PATH );
    image.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    image.SetProperty( Actor::Property::PARENT_ORIGIN, Vector3( 0.5f,  0.25f, 0.5f ) );
    window.Add( image );
}

void OffscreenExample::Terminate()
{
    fprintf(stderr, "OffscreenExample::Terminate()\n");
}