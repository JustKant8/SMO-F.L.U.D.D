#include "actors/FluddNormal.hpp"

namespace ca {

   FluddNormal* FluddNormalInstance;

FluddNormal::FluddNormal(const char *name) : al::LiveActor(name) { }

typedef void (FluddNormal::*functorType)();

void FluddNormal::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddNormal", nullptr);

    al::initNerve(this, &nrvFluddNormalWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
    
}

void FluddNormal::connect(LiveActor* f) {
    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, f) > 50.0f) {
        al::setTrans(this, al::getTrans(f));
        al::attachMtxConnectorToJoint(mtxConnector, f, "nozzle_center");
    }
}

void FluddNormal::activate(bool startEffect) {

    al::tryStartActionIfNotPlaying(this, "Shoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);

    if (startEffect) {
        //al::emitEffect(this, "WaterBall", al::getTransPtr(this));
        //al::startSe(this, "Attack");
    }
}

void FluddNormal::deactivate() {
    //al::tryDeleteEffect(this, "WaterBall");
    al::tryStartActionIfNotPlaying(this, "normal_wg_house");
}

void FluddNormal::initAfterPlacement() {

}

void FluddNormal::listenAppear() {
    this->appear();
}

bool FluddNormal::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void FluddNormal::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void FluddNormal::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);

    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
    
}

void FluddNormal::updateCollider() {
    
    
}


void FluddNormal::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(FluddNormal, Wait)
} // namespace

} // namespace ca