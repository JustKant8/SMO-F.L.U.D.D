#include "actors/FluddHover.hpp"

namespace ca {

   FluddHover* fluddHoverInstance;

FluddHover::FluddHover(const char *name) : al::LiveActor(name) { }

typedef void (FluddHover::*functorType)();

void FluddHover::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddHover", nullptr);

    al::initNerve(this, &nrvFluddHoverWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
    
}

void FluddHover::connect(LiveActor* f) {
    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, f) > 50.0f) {
        al::setTrans(this, al::getTrans(f));
        al::attachMtxConnectorToJoint(mtxConnector, f, "nozzle_center");
    }
}

void FluddHover::activate(bool startEffect) {

    al::tryStartActionIfNotPlaying(this, "Shoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);

    if (startEffect) {
        al::startSe(this, "WaterRoadMove_loop");//PgWaterEmit
    }
}

void FluddHover::deactivate() {
    al::stopSe(this, "WaterRoadMove_loop", 0, nullptr);
    al::setSklAnimFrame(this, 0, 0);
}

void FluddHover::initAfterPlacement() {

}

void FluddHover::listenAppear() {
    this->appear();
}

bool FluddHover::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void FluddHover::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void FluddHover::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);
    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
    
}

void FluddHover::updateCollider() {
    
    
}


void FluddHover::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(FluddHover, Wait)
} // namespace

} // namespace ca