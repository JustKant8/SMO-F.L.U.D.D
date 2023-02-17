#pragma once

#include "al/camera/CameraPoser.h"
#include "al/camera/alCameraPoserFunction.h"
#include "al/camera/CameraAngleVerticalCtrl.h"

#include "sead/math/seadVector.h"

#include "al/util.hpp"

// cc = custom cameras

namespace cc {
    class CameraPoserCustom : public al::CameraPoser {
        public:
            CameraPoserCustom(char const*);
            virtual void start(al::CameraStartInfo const&) override;
            virtual void init() override;
            void reset(void);
            virtual void update(void) override;
            virtual void movement(void) override;

            float mOffsetY = 80.0f; // 0x140 ////120
            float mDist = 1000.0f; // 0x144
            float mAngle = 20.0f;   // 0x148
    };
}