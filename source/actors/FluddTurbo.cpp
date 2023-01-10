#include "actors/FluddTurbo.hpp"

namespace ca {

   FluddTurbo* fluddTurboInstance;

FluddTurbo::FluddTurbo(const char *name) : al::LiveActor(name) { }

typedef void (FluddTurbo::*functorType)();

void FluddTurbo::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddTurbo", nullptr);

    al::initNerve(this, &nrvFluddTurboWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
    
}

void FluddTurbo::activate(al::LiveActor* fludd, bool startEffect) {
    // al::setTrans(this, al::getTrans(mario));

    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, fludd) > 50.0f) {
        al::setTrans(this, al::getTrans(fludd));
        al::attachMtxConnectorToJoint(mtxConnector, fludd, "nozzle_center");
    }

    al::tryStartActionIfNotPlaying(this, "OldShoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);

    if (startEffect) {
        if (effectDelay >= 1) {
            al::emitEffect(this, "WaterBall", al::getTransPtr(this));
            effectDelay = 0;
        } else {
            effectDelay += 1;
        }
        al::startSe(this, "Attack");
    }
}

void FluddTurbo::deactivate() {
    al::tryDeleteEffect(this, "WaterBall");
    //al::stopAction(this);
    al::setSklAnimFrame(this, 0, 0);
}

void FluddTurbo::initAfterPlacement() {

}

void FluddTurbo::listenAppear() {
    this->appear();
}

bool FluddTurbo::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void FluddTurbo::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void FluddTurbo::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);

    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
    
}

void FluddTurbo::updateCollider() {
    
    
}


void FluddTurbo::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(FluddTurbo, Wait)
} // namespace

} // namespace ca