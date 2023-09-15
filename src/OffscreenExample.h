#ifndef OFFSCREEN_EXAMPLE_H
#define OFFSCREEN_EXAMPLE_H

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/adaptor-framework/offscreen-application.h>
#include <dali/devel-api/adaptor-framework/offscreen-window.h>

extern void DumpTbmToFile(void* data);
extern void DumpTbmToBuffer(void* data);

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
    bool OnTick();
    void OnFrameRendered();

private:
    OffscreenApplication& mApplication;
};
#endif