#include "actors/RocketItem.hpp"

namespace ca {

RocketItem::RocketItem(const char* name) : al::LiveActor(name) {}

typedef void (RocketItem::*functorType)();

void RocketItem::init(al::ActorInitInfo const& info) {
    al::initActorWithArchiveName(this, info, "RocketItem", nullptr);

    al::initNerve(this, &nrvRocketItemWait, 0);

    al::setSensorRadius(this, "Explosion", 0.0f);

    al::invalidateHitSensor(this, "Explosion");

    mtxConnector = al::createMtxConnector(this);

    this->makeActorAlive();

    al::hideModelIfShow(this);
}

void RocketItem::connect() {
    if (!al::isMtxConnectorConnecting(mtxConnector) || al::calcDistance(this, f) > 50.0f) {
        //al::invalidateCollisionParts(this);
        al::setTrans(this, al::getTrans(f));
        //al::validateCollisionParts(this);
        al::attachMtxConnectorToJoint(mtxConnector, f, "jnt_muzzle_3");
        //posOffset = al::getFront(this);
        
    }
}

void RocketItem::shoot(bool ready) {
    
    if (!ready && !shooting) {  // waiting
        connect();
        al::showModelIfHide(this);
        al::tryStartActionIfNotPlaying(this, "rocket_base");
        al::invalidateHitSensor(this, "Explosion");
        al::invalidateHitSensor(this, "Attack");
    } else if (!shooting) {  // shooting
        al::tryStartActionIfNotPlaying(this, "rocket_fly");

            if (al::isMtxConnectorConnecting(mtxConnector))
                al::disconnectMtxConnector(mtxConnector);

            

            al::validateHitSensor(this, "Body");

            //al::validateHitSensor(this, "Explosion");
            //al::setSensorRadius(this, "Explosion", 100.0f);

            //al::setRotateZ(this, 0.0f);

            al::setVelocityToFront(this, 25.0f);
            timer = 300;
            shooting = true;
        }

    if (isSklAnimEnd(this, 0))
        al::setSklAnimFrame(this, 0, 0);
}

void RocketItem::deactivate() {
    if (shooting) {
        if (timer > 0) {
            timer -= 1;
            al::emitEffect(this, "FlySmoke", al::getTransPtr(this));
            al::startSe(this, "Flying");
            isExplode = false;
        } else {
            al::setNerve(this, &nrvRocketItemExplode);
        }
    } else {
        al::hideModelIfShow(this);
        al::disconnectMtxConnector(mtxConnector);
        al::stopSe(this, "Flying", 0, nullptr);
        al::tryDeleteEffect(this, "FlySmoke");
        isExplode = true;
    }
}

void RocketItem::reset() {
    al::showModelIfHide(this);
}

bool RocketItem::receiveMsg(const al::SensorMsg* message, al::HitSensor* source, al::HitSensor* target) {
   
    if (rs::isMsgTargetMarkerPosition(message)) {
        sead::Vector3f& transPtr = al::getTrans(this);
        rs::setMsgTargetMarkerPosition(message, sead::Vector3f(transPtr.x + 0.0, transPtr.y + 180.0f, transPtr.z + 0.0));
        return true;
    }

    if (al::isMsgFloorTouch(message) || al::isMsgHit(message) || rs::isMsgDonsukeAttack(message)) {
        msgReceived = true;
        return true;
    }

    return false;


    //if (!shooting || al::isHideModel(this) || al::isSensorPlayer(source)) {
     //   msgReceived = false;
    //    return false;
    //}

    //rs::setAppearItemFactorAndOffsetByMsg(this, message, source);
    //rs::requestHitReactionToAttacker(message, target, source);
    //al::startHitReaction(this, "死亡");
    //explode();
    
    //al::validateHitSensor(this, "Explosion");
    //al::setSensorRadius(this, "Explosion", 100.0f);

    //if (al::isMsgFloorTouch(message) || al::isMsgHit(message) && al::isSensorEnemyAttack(source) ||
    //    al::isMsgPlayerObjTouch(message) && al::isSensorEnemyAttack(source)) {
    //    if (isAttacking) {
    //        al::sendMsgEnemyAttackFire(source, target, nullptr);
    //        msgReceived = true;
    //        return true;
    //    }
   // }
}

void RocketItem::attackSensor(al::HitSensor* source, al::HitSensor* target) {

    if (al::isSensorName(source, "Body") && !al::isSensorPlayerAll(source) && !al::isNearPlayer(this, 500)) {
        al::setNerve(this, &nrvRocketItemExplode); 
    }

    if (al::isNerve(this, &nrvRocketItemExplode) && al::isSensorName(source, "Explosion")) {
        al::sendMsgExplosion(target, source, nullptr);
        al::sendMsgBallAttack(target, source, nullptr);
        al::sendMsgHitStrong(target, source);
        al::sendMsgKillerAttack(target, source);
        al::sendMsgPlayerAttackTrample(target, source, nullptr); //sendMsgPlayerDamageTouch , sendMsgPlayerGiantHipDrop
        al::sendMsgHammerBrosHammerHackAttack(target, source);
        isAttacking = true;
        return;
    }
    
    isAttacking = false;

     return;
}


void RocketItem::control() {
    al::connectPoseQT(this, mtxConnector, *quat, *posOffset);

    if (al::isInDeathArea(this) || al::isCollidedFloorCode(this, "DamageFire") ||
        al::isCollidedFloorCode(this, "Needle") || al::isCollidedFloorCode(this, "Poison")) {
        al::setNerve(this, &nrvRocketItemExplode);
    }

    isConnecting = al::isMtxConnectorConnecting(mtxConnector);
}

void RocketItem::updateCollider() {
    sead::Vector3f& velocity = al::getVelocity(this);

    if (!shooting) {
        al::invalidateColliderRobustCheck(this);
        if (al::isNoCollide(this)) {
            *al::getTransPtr(this) += velocity;
            al::getActorCollider(this)->onInvalidate();
        } else {
            sead::Vector3f result = al::getActorCollider(this)->collide(velocity);
            *al::getTransPtr(this) += result;
        }
        colDelay = 20;
    } else {
        colDelay -= 1;
        if (colDelay <= 0) {
            al::validateColliderRobustCheck(this);
            if (al::isNoCollide(this)) {
                *al::getTransPtr(this) += velocity;
                al::getActorCollider(this)->onInvalidate();
            } else {
                sead::Vector3f result = al::getActorCollider(this)->collide(velocity);
                *al::getTransPtr(this) += result;
            }
        }
    }
}

void RocketItem::exeWait() {

}

void RocketItem::listenAppear() {
    this->appear();
}

void RocketItem::updateVelocity() {}

void RocketItem::exeExplode(void) {
    if (al::isFirstStep(this)) {
        al::setVelocityZero(this);

        al::validateHitSensor(this, "Explosion");
        al::tryEmitEffect(this, "Explosion", al::getTransPtr(this));
        al::setEffectAllScale(this, "Explosion", sead::Vector3f(1.0f, 1.0f, 1.0f));
        al::startSe(this, "EmBombExplode");
    }

        al::setSensorRadius(this, "Explosion", 200.0f);

    explodeDelay -= 1;

    if (explodeDelay <= 0) {  // 

        explodeDelay = 30;

        al::invalidateHitSensor(this, "Body");
        al::setSensorRadius(this, "Explosion", 0.0f);
        al::invalidateHitSensor(this, "Explosion");

        //al::tryDeleteEffect(this, "Explosion");

        shooting = false;
        isExplode = true;
        msgReceived = false;
        al::hideModelIfShow(this);
        al::setNerve(this, &nrvRocketItemWait);
    }    
}


namespace
{
NERVE_IMPL(RocketItem, Wait)
NERVE_IMPL(RocketItem, Explode)
} // namespace

} // namespace ca