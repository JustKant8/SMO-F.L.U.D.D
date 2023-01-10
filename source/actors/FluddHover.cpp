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

void FluddHover::activate(al::LiveActor* fludd, bool startEffect) {
    // al::setTrans(this, al::getTrans(mario));

    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, fludd) > 50.0f) {
        al::setTrans(this, al::getTrans(fludd));
        al::attachMtxConnectorToJoint(mtxConnector, fludd, "nozzle_center");
    }

    al::tryStartActionIfNotPlaying(this, "Shoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);

    if (startEffect) {
        al::startSe(this, "PgWaterEmit");//PgWaterEmit
    }
}

void FluddHover::deactivate() {
    al::stopSe(this, "PgWaterEmit", 0, nullptr);
    //al::stopAction(this);
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