#pragma once

#include <sead/math/seadVector.h>

#include "al/camera/CameraDirector.h"

namespace al {
    sead::Vector3f *getCameraUp(al::IUseCamera const *, int);
    void startCamera(al::IUseCamera const*, al::CameraTicket*, int priority);
    void endCamera(al::IUseCamera const*, al::CameraTicket*, int, bool);
}