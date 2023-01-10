#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/async/FunctorV0M.hpp"
#include "al/physics/ExternalForceKeeper.h"
#include "al/states/EnemyStateWander.h"
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
#include "game/Player/HackCap/HackCapJointControlKeeper.h"
#include "logger.hpp"

namespace ca {

    class FluddBase;

    class FluddBase : public al::LiveActor {
    public:
        FluddBase(char const* name);
        virtual void init(al::ActorInitInfo const& info);
        virtual void initAfterPlacement(void);
        void listenAppear();
        virtual bool receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target);
        virtual void attackSensor(al::HitSensor* source, al::HitSensor* target);
        virtual void control() override;
        virtual void updateCollider();

        //Mtx connection/effects
        void activate(PlayerActorHakoniwa* mario);

        void deactivate();

        void exeWait(void);

        
        bool isConnecting;
        private:
        al::MtxConnector* mtxConnector;

        //for better model placement
        sead::Quatf* quat = new sead::Quatf(0, 0, 0, 0);
        sead::Vector3f* posOffset = new sead::Vector3f(-5, 4, 0);
    };

    namespace {
    NERVE_HEADER(FluddBase, Wait)
    }  // namespace
}


