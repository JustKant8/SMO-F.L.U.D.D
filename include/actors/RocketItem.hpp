#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/async/FunctorV0M.hpp"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "al/util/SensorUtil.h"
#include "rs/util/SensorUtil.h"
#include "al/util/MathUtil.h"
#include "game/Player/PlayerActorHakoniwa.h"

#include <sead/basis/seadTypes.h>
#include <sead/container/seadPtrArray.h>
#include <sead/heap/seadHeap.h>
#include <sead/math/seadVector.h>
#include "al/MtxConnector/MtxConnector.h"
#include "al/util.hpp"
#include "actors/FluddNormal.hpp"

#include "logger.hpp"

namespace ca {

    class RocketItem;

    class RocketItem : public al::LiveActor {
    public:
        RocketItem(char const* name);
        virtual void init(al::ActorInitInfo const& info);
        void listenAppear();
        virtual void attackSensor(al::HitSensor* source, al::HitSensor* target) override;
        virtual bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) override;
        
        virtual void control() override;
        virtual void updateCollider();

        void updateVelocity();
        void reset();

        void exeWait();

        void exeExplode(void);

        void shoot(bool);

        void deactivate();

        void connect();

        bool isConnecting = false;

        bool shooting = false;
        bool isExplode = false;

        bool isAttacking = false;
        bool msgReceived = false;

        int explodeDelay = 30;

        int colDelay = 20;

        int timer = 300; //5 seconds

        ca::FluddNormal* f;

    private:
        al::MtxConnector* mtxConnector;
        sead::Quatf* quat = new sead::Quatf(0.707, 0, 0.707, 0);
        sead::Vector3f* posOffset = new sead::Vector3f(30, 0, 0);
    };

    namespace {
    NERVE_HEADER(RocketItem, Wait)
    NERVE_HEADER(RocketItem, Explode)
    }  // namespace
}


