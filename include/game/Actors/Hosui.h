#pragma once

#include "al/LiveActor/LiveActor.h"

class Hosui : public al::LiveActor {
public:
    Hosui(const char* name);
    void init(al::ActorInitInfo const& info);
    void shootHorizontal(uint);
    void updateWaterBall(sead::Vector3f const&, uint, bool);

};