#ifndef OFFSCREEN_EXAMPLE_H
#define OFFSCREEN_EXAMPLE_H

#include <common/common.h>
#include <memory> ///< for std::unique_ptr

#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>

using namespace Dali;
using namespace Dali::Toolkit;

class OffscreenExample : public ConnectionTracker
{
public:
    OffscreenExample(OffscreenApplication& application);
    ~OffscreenExample();

private:
    void Create();
    void Terminate();

private:
    OffscreenApplication& mApplication;
};
#endif