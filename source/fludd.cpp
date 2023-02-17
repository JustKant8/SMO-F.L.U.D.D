#include "fludd.h"
#include "al/LiveActor/LiveActor.h"
#include "al/scene/Scene.h"
#include "al/util.hpp"
#include "al/util/ControllerUtil.h"
#include "al/util/LiveActorUtil.h"
#include "game/GameData/GameDataFunction.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "gfx/seadColor.h"
#include "math/seadMathCalcCommon.h"
#include "math/seadVector.h"
#include "prim/seadSafeString.h"
#include "game/Player/PlayerFunction.h"
#include "rs/util.hpp"
#include <cmath>
#include "al/factory/ActorFactory.h"
#include "helpers.hpp"
#include "al/util/CameraUtil.h"
#include "cameras/CameraPoserCustom.h"

FLUDD& Fludd() {
    static FLUDD f;
    return f;
}

void FLUDD::initModels(al::ActorInitInfo const& info) {
    base = new ca::FluddBase("Fludd");
    base->init(info);

    turbo = new ca::FluddTurbo("Turbo");
    turbo->init(info);

    hover = new ca::FluddHover("Hover");
    hover->init(info);

    rocket = new ca::FluddRocket("Rocket");
    rocket->init(info);

    jetOne = new ca::HoverWater("jetOne");
    jetOne->init(info);

    jetTwo = new ca::HoverWater("jetTwo");
    jetTwo->init(info);

    normal = new ca::FluddNormal("Normal");
    normal->init(info);

    rocketItem = new ca::RocketItem("RocketItem");
    rocketItem->init(info);                                           
    al::setScaleAll(rocketItem, .7f);
    rocketItem->f = normal;
}

bool FLUDD::isTankEmpty(float f1) {
    if (tank <= f1) {
        tank = f1;

    recharging = true;
        return true;
    } else {

        return false;
    }   
}

void FLUDD::fluddTankFill()
{
    tank = 100.0f;
}

void FLUDD::changeFluddModeL()
{
    fluddMode++;
    //reset panes
    if (layout) {
        resetLayout();
    }
    setFluddModeValues();
}

void FLUDD::changeFluddModeR()
{
    fluddMode--;
    //reset panes
    if (layout) {
        resetLayout();
    }
    
    setFluddModeValues();
}

void FLUDD::resetLayout()
{
    if (!al::isHidePane(layout, "FLUDDHover"))
    {
        al::hidePane(layout, "FLUDDHover");
    }
    if (!al::isHidePane(layout, "FLUDDRocket"))
    {
        al::hidePane(layout, "FLUDDRocket");
    }
    if (!al::isHidePane(layout, "FLUDDTurbo"))
    {
        al::hidePane(layout, "FLUDDTurbo");
    }
    if (!al::isHidePane(layout, "ChargeBar")) {
        al::hidePane(layout, "ChargeBar");
    }
    //add normal mode layout
    if (!al::isHidePane(layout, "FLUDDNormal")) {
        al::hidePane(layout, "FLUDDNormal");
    }
}

void FLUDD::setFluddModeValues()
{
    if (fluddMode == 0 || fluddMode > 3) {  // Hover Fludd
        fluddMode = 0;
        fluddVel = 2.8f; //orig 2.5
        fluddRecharge = 0.4f;
        fluddDischarge = 0.1f;
        chargeTimer = 0.0f;
        tankRunoutVal = 34.0f;
        tStopValueSet = false;
        recharging = false;
        if (!isHack && !is2D && !rs::isActiveDemo(mario)) {
            al::showModelIfHide(hover);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(turbo);
            al::hideModelIfShow(normal);                    
        }
    }
    else if (fluddMode == 1) {  // Rocket Fludd
        fluddVel = 90.0f;
        fluddRecharge = 0.4f;
        chargeTimer = 100.0f;
        chargeTimerDecrease = 1.5f; 
        tankRunoutVal = 33.3f;
        fluddDischarge = tankRunoutVal / 2;  // "/"2 for frame perfect double rocket boost
        tStopValueSet = false;
        recharging = false;
        isFirstBoost = true;
        if (!isHack && !is2D && !rs::isActiveDemo(mario)) {
            al::showModelIfHide(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(turbo);
            al::hideModelIfShow(normal);                         
        }
    } else if (fluddMode == 2) {  // Turbo Fludd           
        fluddMode = 2;
        fluddDischarge = 0.12f;
        fluddRecharge = 0.4f;
        chargeTimer = 100.0f;
        chargeTimerDecrease = 1.0f;
        tankRunoutVal = 100.0f;
        tStopValueSet = false;
        recharging = false;
        if (!isHack && !is2D && !rs::isActiveDemo(mario)) {
            al::showModelIfHide(turbo);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(normal);                     
        }
    }  else if (fluddMode == 3 || fluddMode < 0) {
        fluddMode = 3;
        tankRunoutVal = 20.0f;
        fluddDischarge = tankRunoutVal;
        fluddRecharge = 0.8f;
        chargeTimer = 100.0f;
        chargeTimerDecrease = 2.0f;
        tStopValueSet = false;                                     
        recharging = false;
        if (!isHack && !is2D && !rs::isActiveDemo(mario)) {
            al::showModelIfHide(normal);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(turbo);
        }
        
    }
}

void FLUDD::activateFludd() {

     if (chargeTimer == 0.0f && fluddMode != 3) {
        rocketItem->deactivate();                               
        if (fluddMode == 0){//HOVER
             if (!is2D) {
                 al::setVelocityY(mario, smoothVelocity(al::getVelocity(mario).y, fluddVel, -1));
             } else {
                 sead::Vector3f g = al::getGravity(mario);
                     al::setVelocityY(mario, smoothVelocity(al::getVelocity(mario).y, -1 * g.y * fluddVel, g.y));
             }
            if (mario->mPlayerAnimator->isAnim("JumpBroad") ||
                mario->mPlayerAnimator->isAnim("JumpBroad2") ||
                mario->mPlayerAnimator->isAnim("JumpBroad3") ||
                mario->mPlayerAnimator->isAnim("HeadSliding") ||
                mario->mPlayerAnimator->isAnim("DiveInWater") ||
                mario->mPlayerAnimator->isAnim("HeadSlidingStart")) {
                mario->mPlayerAnimator->startAnim(action);
            }
            base->activate();
            hover->activate(true);  // hover effects
            if (!is2D) {
                jetOne->activate(hover, "jnt_nozzle_R", true);
                jetTwo->activate(hover, "jnt_nozzle_L", true);
            }
            tank -= fluddDischarge;
        } else if (fluddMode == 1 && !isPGrounded) {  // ROCKET
            if (al::isPadHoldA(-1)) {
                if (isFirstBoost) {
                    al::setVelocityY(mario, fluddVel);  // boost velocity
                    if (mario->mPlayerAnimator->isAnim("JumpBroad") ||
                        mario->mPlayerAnimator->isAnim("JumpBroad2") ||
                        mario->mPlayerAnimator->isAnim("JumpBroad3") ||
                        mario->mPlayerAnimator->isAnim("HeadSliding") ||
                        mario->mPlayerAnimator->isAnim("DiveInWater") ||
                        mario->mPlayerAnimator->isAnim("HeadSlidingStart")) {
                        mario->mPlayerAnimator->startAnim(action);
                    }
                    if (!is2D) {
                        base->activate();
                        rocket->activate(true);  // rocket effects
                    }
                    lJCancel = true;

                    if (isFirstBoost) {
                        tank -= fluddDischarge;
                        isFirstBoost = false;
                    }
                } else {
                    doubleBoostFrames += 1;
                    if (doubleBoostFrames >= 1) {
                        tank -= fluddDischarge;
                    }
                }
            } else {
                if (doubleBoostFrames < 1 && !isFirstBoost) {
                    isFirstBoost = true;
                    doubleBoostFrames = 0;
                }
            }
        }
            else if (fluddMode == 2) {  // TURBO
                if (isPInWater) {
                    al::setVelocityToFront(mario, 30.0f);  // movement speed(water)

                    if (al::isPadHoldA(-1)) {
                        al::setVelocityY(mario, 8.0f);
                    } else if (al::isPadHoldB(-1)) {
                        al::setVelocityY(mario, -8.0f);
                    }
                    base->activate();

                    if (isPGrounded) {
                        turbo->activate(true);
                    } else {
                        turbo->activate(false);
                        al::emitEffect(
                            mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),
                            "StateIce", al::getTransPtr(mario));
                    }

                    tank -= fluddDischarge / 2;
                } else if (isPGrounded) {
                    if (setNrvGrounded) {
                        mario->setNerveOnGround();
                        setNrvGrounded = false;
                    }
                    al::setVelocityToFront(mario, 45.0f);  // movement speed(ground)   ___orig 40___
                    al::setVelocityY(mario, -5.0f);
                    base->activate();
                    if (!is2D) {
                        turbo->activate(true);  // turbo effects
                    }
                    tank -= fluddDischarge;
                }
            
            }
            
    } else if (fluddMode == 3) {
         //Fludd mode 3(fire rocket)

         //shoot rocket item
            normal->activate(true);
            rocketItem->shoot(chargeTimer == 0);
            if (rocketItem->shooting && rocketItem->isExplode)                 
                tank -= fluddDischarge;

     } else {
        rocketItem->deactivate();
    }
    
}

float FLUDD::smoothVelocity(float from, float to, float g) { 

    float newVel = 0.0f;
    
    if (g < 0) { //normal gravity
        if ((to + 2) < from)
            newVel = from;
        else if ((to + 2) > from)
            newVel = sead::lerp(from, to, 0.8f);
        else
            newVel = to;
    } else if (g > 0) { //reverse gravity
        if ((to + 2) > from)
            newVel = from;
        else if ((to + 2) < from)
            newVel = sead::lerp(from, to, 0.8f);
        else
            newVel = to;
    }
    return newVel;
}

void FLUDD::deactivateFludd()
{ 
    //deactivates effects(sound/visual)
    base->deactivate();
    hover->deactivate();
    if (al::getVelocity(mario).y < 5)
        rocket->deactivate();
    turbo->deactivate();
    jetOne->deactivate();
    jetTwo->deactivate();
    normal->deactivate();                     
    rocketItem->deactivate();
    al::tryDeleteEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),"StateIce");

        if (chargeTimer < 100.0f && fluddMode != 0) {  // recharge chargeup timer
            if (fluddMode != 3) {
                chargeTimer += 0.6f;
            } else {
                chargeTimer += 0.33f;
            }
        }

        if (isPGrounded || isPInWater) {
            if (isPInWater) {
                fluddTankFill();            //added
            }

            // recharge tank
            if (tank < 100.0f && fluddMode == 0) {
                tank += fluddRecharge;
            } else if (tank < 100.0f && fluddMode != 0 && chargeTimer >= 100.0f) {
                chargeTimer = 100.0f;
                tank += fluddRecharge;
            }
            // Check if fludd can activate or not
            if (fluddMode == 0) {
                recharging = false;
            } else if (fluddMode != 0 && chargeTimer >= 100.0f) {
                chargeTimer = 100.0f;
               
                isFirstBoost = true;
                doubleBoostFrames = 0;
                recharging = false;
            }

            if (!recharging && tStopValueSet) {
                tStopValueSet = false;
            }
        }
    
}

float FLUDD::stopTankValue() {
    float val = 0.0f;
    if (tank >= tankRunoutVal)
        val = tank - tankRunoutVal;
    else
        val = 0.0f;

    return val;
}

void FLUDD::updateLayout() {

    if (stickActive) {
        al::showPane(layout, "LStick");
    } else {
        al::hidePane(layout, "LStick");
    }

    if (fluddMode == 0)
    {
        al::showPane(layout, "FLUDDHover");
    }
    else if (fluddMode == 1)
    {
        al::showPane(layout, "FLUDDRocket");
        if (chargeTimer < 100.0f) {
            al::showPane(layout, "ChargeBar");
        } else {
            al::hidePane(layout, "ChargeBar");
        }
    }
    else if (fluddMode == 2)
    {
        al::showPane(layout, "FLUDDTurbo");
        if (chargeTimer < 100.0f) {
            al::showPane(layout, "ChargeBar");
        } else {
            al::hidePane(layout, "ChargeBar");
        }
    } else if (fluddMode == 3) {
        al::showPane(layout, "FLUDDNormal");
        if (chargeTimer < 100.0f) {
            al::showPane(layout, "ChargeBar");
        } else {
            al::hidePane(layout, "ChargeBar");
        }
    }
    float tankVal = tank/100.0f;//what percentage is tank for layout scale/transform
    float tankStopVal = tankStopValue / 100.0f; //little stop bar
    float cT = chargeTimer / 100.0f;//chargeTimer percentage

    float tankTrans = -39.0f - (118*(1-tankVal));
    float tankScale = tankVal;
    float tankStopLevel = -160 + (237 * tankStopVal);
    float cBarTrans = -43 - (119*(1-cT));
    float cBarScale = cT;

    //stop level layout pos
     tankStopLevelPos = sead::Vector2f(-10.4f, tankStopLevel);
     al::setPaneLocalTrans(layout, "TankWaterStop", tankStopLevelPos);
    //tank water trans
    tankWaterTrans = sead::Vector2f(3.5f, tankTrans);
    al::setPaneLocalTrans(layout, "TankWater", tankWaterTrans);
    //tank water scale
    tankWaterScale = sead::Vector2f(1.0f, tankScale);
    al::setPaneLocalScale(layout, "TankWater", tankWaterScale);

    //chargeBar trans/scale
    chargeBarTrans = sead::Vector2f(0.0f, cBarTrans);
    al::setPaneLocalTrans(layout, "ChargeBar", chargeBarTrans);
    chargeBarScale = sead::Vector2f(1.0f, cBarScale);
    al::setPaneLocalScale(layout, "ChargeBar", chargeBarScale);
    
}

void FLUDD::setMarioPtr(PlayerActorHakoniwa* m) {
    mario = m;
}

void FLUDD::updateFludd() {
    //set all references

    

    if (mario) {
        
        setRefs();

        //rolling effect
        if (mario->mPlayerAnimator->curAnim == "Rolling" && isPGrounded) {
            al::emitEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),
                          "MofumofuDemoOpening2LandWet", al::getTransPtr(mario)); 
        } else {
            al::tryDeleteEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),
                                "MofumofuDemoOpening2LandWet");  // DashFastPuddle
        }

        
        // changing fludds mode
        if (stickActive && al::isPadTriggerPressLeftStick(-1) && !al::isPadHoldL(-1)) { //left stick press(toggle)
            changeFluddModeL();
        }
        if (al::isPadTriggerLeft(-1)) { //left d-pad
            changeFluddModeL();
        }
        if (al::isPadTriggerRight(-1)) { //right d-pad
            changeFluddModeR();
        }
        if (al::isPadHoldL(-1) && al::isPadTriggerPressLeftStick(-1)) { //toggle left stick press
            stickActive = !stickActive;
        }
        
        

        //Stage/scene change fludd setup
        if (stageChange) {
            firstTimeSetup();
            stageChange = false;
        }

        // new fludd setup
        updateModels();
        
        // fludd layout
        if (layout->mIsAlive) {
            updateLayout();
        }

        //sets stop value (black bar on UI)
        if (!tStopValueSet) {
            tankStopValue = stopTankValue();
            tStopValueSet = true;
        }
        
        //forces grounded nerve when turbo is active and on ground(bug fix)
        if (!isPGrounded && fluddMode == 2) {
            setNrvGrounded = true;
        }

        //cancels long jump nerve when rocket is activated(bug fix)
        if (!is2D && isTankEmpty(tankStopValue) && lJCancel) {
            lJCancel = false;
            mario->setNerveOnGround();
        }

        //rocket item controls
         if (rocketItem->shooting) {

            rocketCamera->mPoser->mTargetTrans = al::getTrans(rocketItem);
            if (!al::isActiveCamera(rocketCamera)) {
                al::startCamera(rocketItem, rocketCamera, -1);
            }
            
            sead::Vector2f input = *al::getRightStick(-1);
            bool noInput = true;

            if (input.x > 0.3) {
                al::rotateQuatYDirDegree(rocketItem, 2);
                noInput = false;
            } else if (input.x < -0.3) {
                al::rotateQuatYDirDegree(rocketItem, -2);
                noInput = false;
            }
                                                                                   
            if (input.y > 0.3) {
                al::rotateQuatXDirDegree(rocketItem, 1);
                noInput = false;
            } else if (input.y < -0.3) {
                al::rotateQuatXDirDegree(rocketItem, -1);
                noInput = false;
            }

            if (noInput) {
                al::setVelocityToFront(rocketItem, 30);
            } else {
                al::setVelocityToFront(rocketItem, 25);
            }
            
        } else if (al::isActiveCamera(rocketCamera)) {
            al::endCamera(rocketItem, rocketCamera, -1, false);
            cc::CameraPoserCustom* rCamera = (cc::CameraPoserCustom*)rocketCamera->mPoser;
            rCamera->mAngle = 20.0f;
        }
        
        // starts fludd
        if (al::isPadHoldR(-1) && !recharging) {
            if (canFluddActivate()) {
                activateFludd();  // fludd activation
            } else if (!rocketItem->shooting) {
                al::hideModelIfShow(rocketItem);                  
            }
        }
        else {
            deactivateFludd();  // fludd deactivation
        }
        
    }
    
}

int FLUDD::getFluddMode() {
    return fluddMode;
}

void FLUDD::setRefs() {
    al::LiveActor* hack = mario->mHackKeeper->currentHackActor;
    if (hack) {
        isHack = true;
    } else {
        isHack = false;
    }
    marioModel = mario->mPlayerAnimator->mModelHolder->currentModel->mLiveActor;  // currentModel->mLiveActor
    is2D = mario->mDimKeeper->is2D;
    isPGrounded = rs::isPlayerOnGround(mario);
    layout = stageSceneRef->stageSceneLayout->mCoinCountLyt;
    isPUnderWater = rs::isPlayerInWater(mario);
    isPInWater = al::isInWaterPos(mario, al::getTrans(mario));
}

void FLUDD::firstTimeSetup() { // starts anim to fix rotation issue
    base->activate();
    setFluddModeValues();
    resetLayout();
    hover->activate(false);
    rocket->activate(false);
    turbo->activate(false);
    normal->activate(false);
    rocketItem->shoot(false);                     
    rocketItem->connect();
    
}

bool FLUDD::canFluddActivate() {
    
    if (!isHack && !PlayerFunction::isPlayerDeadStatus(mario) && !rs::isActiveDemo(mario) && !isTankEmpty(tankStopValue)) { //&&!recharging

        if (chargeTimer > 0.0f) {
            chargeTimer -= chargeTimerDecrease;
        } else
            chargeTimer = 0.0f;

        if (fluddMode == 0)  // HOVER
        {
            rocket->deactivate();
            turbo->deactivate();
            normal->deactivate();                       
            if (!isPGrounded && !isPUnderWater) {
                return true;
            } else if (isPGrounded) {
                hover->deactivate();
                jetOne->deactivate();
                jetTwo->deactivate();
            }
        }
        if (fluddMode == 1)  // ROCKET
        {
            jetOne->deactivate();
            jetTwo->deactivate();
            turbo->deactivate();
            hover->deactivate();
            normal->deactivate();                 
            if (!isPInWater && !isPGrounded) {
                return true;
            }
        }
        if (fluddMode == 2)  // TURBO
        {
            jetOne->deactivate();
            jetTwo->deactivate();
            rocket->deactivate();
            hover->deactivate();
            normal->deactivate();                     
            if (mario->mPlayerInput->isMove()) {  // checks if mario is moving
                    return true;
            }
        }
        if (fluddMode == 3) {
            jetOne->deactivate();
            jetTwo->deactivate();
            rocket->deactivate();                      
            hover->deactivate();
            turbo->deactivate();
        
            return true;
        }
        
       
    }
    else if (isTankEmpty(tankStopValue)) {
        recharging = true;
    }

    return false;
}

void FLUDD::updateModels() {
    if (is2D || isHack || rs::isActiveDemo(mario)) { 
            al::hideModelIfShow(base);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(turbo);
            al::hideModelIfShow(normal);                 
            doOnce = true;
    } else {
        if (doOnce) {
            al::showModelIfHide(base);
            setFluddModeValues();
            doOnce = false;
        }
        base->connect(marioModel);
        hover->connect(base);
        rocket->connect(base);
        turbo->connect(base);
        normal->connect(base);                 
        
    }
}


