#include "actors/FluddBase.hpp"

namespace ca {

FluddBase::FluddBase(const char *name) : al::LiveActor(name) { }

typedef void (FluddBase::*functorType)();

void FluddBase::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddBase", nullptr);

    al::initNerve(this, &nrvFluddBaseWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();
    
}

void FluddBase::activate(PlayerActorHakoniwa* mario) {
    
    //al::setTrans(this, al::getTrans(mario));

    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, mario) > 50.0f) {
        al::setTrans(this, al::getTrans(mario));
        al::attachMtxConnectorToJoint(mtxConnector, mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"), "Spine1");
    }

    al::tryStartActionIfNotPlaying(this, "Shoot");

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0); 
}


void FluddBase::deactivate() {
    al::setSklAnimFrame(this, 0, 0);
    //al::stopAction(this);
    al::setSklAnimFrame(this, 0, 0);
}

void FluddBase::initAfterPlacement() {
    
}

void FluddBase::listenAppear() {
    this->appear();
}

bool FluddBase::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void FluddBase::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void FluddBase::control() {

    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);
    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
}

void FluddBase::updateCollider() {
    
    
}


void FluddBase::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(FluddBase, Wait)
} // namespace

} // namespace ca