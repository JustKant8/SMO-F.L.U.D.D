#include "actors/FluddRocket.hpp"

namespace ca {

   FluddRocket* fluddRocketInstance;

FluddRocket::FluddRocket(const char *name) : al::LiveActor(name) { }

typedef void (FluddRocket::*functorType)();

void FluddRocket::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddRocket", nullptr);

    al::initNerve(this, &nrvFluddRocketWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
    
}

void FluddRocket::connect(LiveActor* f) {
    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, f) > 50.0f) {
        al::setTrans(this, al::getTrans(f));
        al::attachMtxConnectorToJoint(mtxConnector, f, "nozzle_center");
    }
}

void FluddRocket::activate(bool startEffect) {

    al::tryStartActionIfNotPlaying(this, "Shoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);

    if (startEffect) {
        al::emitEffect(this, "WaterRoadMove", al::getTransPtr(this));
        al::startSe(this, "Launch");
    }
}

void FluddRocket::deactivate() {

    al::tryDeleteEffect(this, "WaterRoadMove");
    al::setSklAnimFrame(this, 0, 0);
}

void FluddRocket::initAfterPlacement() {

}

void FluddRocket::listenAppear() {
    this->appear();
}

bool FluddRocket::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void FluddRocket::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void FluddRocket::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);
    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
    
}

void FluddRocket::updateCollider() {
    
    
}


void FluddRocket::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(FluddRocket, Wait)
} // namespace

} // namespace ca