#include "actors/HoverWater.hpp"

namespace ca {


HoverWater::HoverWater(const char *name) : al::LiveActor(name) { }

typedef void (HoverWater::*functorType)();

void HoverWater::init(al::ActorInitInfo const &info)
{
    al::initActorWithArchiveName(this, info, "FluddHover", nullptr);

    al::initNerve(this, &nrvHoverWaterWait, 1);

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
    
}

void HoverWater::activate(al::LiveActor* fludd , const char* jointName, bool startEffect) {
    // al::setTrans(this, al::getTrans(mario));

    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, fludd) > 50.0f) {
        al::setTrans(this, al::getTrans(fludd));
        al::attachMtxConnectorToJoint(mtxConnector, fludd, jointName);
    }

    if (startEffect) {
        al::emitEffect(this, "WaterSquirtL", al::getTransPtr(this));
    }
}

void HoverWater::deactivate() {
    al::tryDeleteEffect(this, "WaterSquirtL");

}

void HoverWater::initAfterPlacement() {
    
}

void HoverWater::listenAppear() {
    this->appear();
}

bool HoverWater::receiveMsg(const al::SensorMsg* message, al::HitSensor* source,
                        al::HitSensor* target) {
    
}

void HoverWater::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    
}

// todo: no idea what 0x144 or 0x124 are

void HoverWater::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);
    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
    
}

void HoverWater::updateCollider() {
    
    
}

void HoverWater::exeWait(void)  // 0x0
{
    if (al::isFirstStep(this)) {
        //al::startAction(this, "Wait");
    }
}

namespace
{
NERVE_IMPL(HoverWater, Wait)
} // namespace

} // namespace ca