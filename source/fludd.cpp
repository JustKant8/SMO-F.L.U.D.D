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

    rocket = new ca::FluddRocket("Turbo");
    rocket->init(info);

    jetOne = new ca::HoverWater("jetOne");
    jetOne->init(info);

    jetTwo = new ca::HoverWater("jetTwo");
    jetTwo->init(info);
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

bool FLUDD::isChargeTimerFull()
{
    if (chargeTimer == 100.0f) {
        return true;
    }
    else
        return false;
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
}

void FLUDD::setFluddModeValues()
{
    if (fluddMode == 0 || fluddMode > 2) {  // Hover Fludd
        fluddMode = 0;
        fluddVel = 2.8f; //orig 2.5
        fluddRecharge = 0.4f;
        fluddDischarge = 0.1f;
        chargeTimer = 0.0f;
        tankRunoutVal = 34.0f;
        tStopValueSet = false;
        recharging = false;
        if (!isHack && !is2D) {
            al::showModelIfHide(hover);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(turbo);
        }
        
    }
    else if (fluddMode == 1) {  // Rocket Fludd
        fluddVel = 90.0f;
        fluddDischarge = tankRunoutVal/2; // "/"2 for frame perfect double rocket boost
        fluddRecharge = 0.4f;
        chargeTimer = 100.0f;
        chargeTimerDecrease = 1.5f; 
        tankRunoutVal = 33.3f;
        tStopValueSet = false;
        recharging = false;
        if (!isHack && !is2D) {
            al::showModelIfHide(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(turbo);
        }
        
    }
    else if (fluddMode == 2 || fluddMode < 0) {  //Turbo Fludd
        fluddMode = 2;
        fluddDischarge = 0.12f;
        fluddRecharge = 0.4f;
        chargeTimer = 100.0f;
        chargeTimerDecrease = 1.0f;
        tankRunoutVal = 100.0f;
        tStopValueSet = false;
        recharging = false;
        if (!isHack && !is2D) {
            al::showModelIfHide(turbo);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(hover);
        }
        
    }
}

void FLUDD::activateFludd() {
    active = true;

     if (chargeTimer == 0.0f) {
        if (fluddMode == 0){//HOVER
             if (!is2D) {
                 al::setVelocityY(mario, smoothVelocity(al::getVelocity(mario).y, fluddVel, -1));  // boost velocity, al::setVelocityY
             } else {
                 sead::Vector3f g = al::getGravity(mario);
                     al::setVelocityY(mario, smoothVelocity(al::getVelocity(mario).y, -1 * g.y * fluddVel, g.y));
             }
            if (mario->mPlayerAnimator->isAnim("JumpBroad") ||
                mario->mPlayerAnimator->isAnim("JumpBroad2") ||
                mario->mPlayerAnimator->isAnim("JumpBroad3") ||
                mario->mPlayerAnimator->isAnim("HeadSliding") ||
                mario->mPlayerAnimator->isAnim("DiveInWater")) {
                mario->mPlayerAnimator->startAnim(action);
            }
            base->activate(mario);
            hover->activate(base, true);  // hover effects
            if (!is2D) {
                jetOne->activate(hover, "jnt_nozzle_R", true);
                jetTwo->activate(hover, "jnt_nozzle_L", true);
            }
            tank -= fluddDischarge;
        }
        else if (fluddMode == 1 && al::isPadHoldA(-1) && !isPGrounded) {// ROCKET
            al::setVelocityY(mario, fluddVel);  // boost velocity
            if (mario->mPlayerAnimator->isAnim("JumpBroad") ||
                mario->mPlayerAnimator->isAnim("JumpBroad2") ||
                mario->mPlayerAnimator->isAnim("JumpBroad3") ||
                mario->mPlayerAnimator->isAnim("HeadSliding") ||
                mario->mPlayerAnimator->isAnim("DiveInWater")) {
                mario->mPlayerAnimator->startAnim(action);
            }
            if (!is2D) {
                base->activate(mario);
                rocket->activate(base, true);  // rocket effects
            }
            lJCancel = true;
            tank -= fluddDischarge;
        }
        else if (fluddMode == 2) {  // TURBO
            if (isPInWater) {
                al::setVelocityToFront(mario, 30.0f);  // movement speed(water)
                
                    if (al::isPadHoldA(-1)) {
                        al::setVelocityY(mario, 6.0f);
                    } else if (al::isPadHoldB(-1)) {
                        al::setVelocityY(mario, -6.0f);
                    }
                    base->activate(mario);
                    
                    if (isPGrounded) {
                        turbo->activate(base, true);
                    }
                    else {
                        turbo->activate(base, false);
                        al::emitEffect(
                            mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),
                            "StateIce", al::getTransPtr(mario));
                    }
                
                tank -= fluddDischarge;
            } else if (isPGrounded) {
                if (setNrvGrounded) {
                    mario->setNerveOnGround();
                    setNrvGrounded = false;
                }
                al::setVelocityToFront(mario, 40.0f);  // movement speed(ground)
                al::setVelocityY(mario, -5.0f);
                base->activate(mario);
                if (!is2D) {
                    turbo->activate(base, true);  // turbo effects
                }
                tank -= fluddDischarge;
            }
            
        }
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
    active = false;
    //deactivates effects(sound/visual)
    base->deactivate();
    hover->deactivate();
    if (al::getVelocity(mario).y < 1)
        rocket->deactivate();
    turbo->deactivate();
    jetOne->deactivate();
    jetTwo->deactivate();
    al::tryDeleteEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"),
                        "StateIce");

    

        if (chargeTimer < 100.0f && fluddMode != 0) {  // recharge chargeup timer
            chargeTimer += 0.6f;
        }

        if (isPGrounded || isPInWater) {
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

void FLUDD::updateTankWaterLyt() {

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

void FLUDD::updateFludd() {
    //set all references
    if (mario) {
        isMario = true;
        setRefs();
        
        //rolling effect
        if (mario->mPlayerAnimator->curAnim == "Rolling" && isPGrounded) {
            al::emitEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"), "MofumofuDemoOpening2LandWet", al::getTransPtr(mario));
        } else {
            al::tryDeleteEffect(mario->mPlayerAnimator->mModelHolder->findModelActor("Normal"), "MofumofuDemoOpening2LandWet");  // DashFastPuddle
        }

        //fludd setup when swap between 2d or hack
        if (isHack || is2D) {
            al::hideModelIfShow(base);
            al::hideModelIfShow(rocket);
            al::hideModelIfShow(hover);
            al::hideModelIfShow(turbo);
            doOnce = true;
        }
        else if (!isHack && !is2D && doOnce) {
            al::showModelIfHide(base);
            setFluddModeValues();
            base->activate(mario);
            hover->activate(base, false);
            rocket->activate(base, false);
            turbo->activate(base, false);
            jetOne->activate(hover, "jnt_nozzle_R", false);
            jetTwo->activate(hover, "jnt_nozzle_L", false);
            deactivateFludd();
            doOnce = false;
        }

        // changing fludds mode
        if (al::isPadTriggerLeft(-1)) {
            changeFluddModeL();
        }
        if (al::isPadTriggerRight(-1)) {
            changeFluddModeR();
        }

        //Stage/scene change fludd setup
        if (stageChange) {
            firstTimeSetup();
            stageChange = false;
        }

        // fludd layout
        if (layout->mIsAlive) {
            updateTankWaterLyt();
        }

        //sets stop value
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

        // starts fludd
        if (al::isPadHoldR(-1) && !recharging) {
            if (canFluddActivate()) {
                activateFludd();  // fludd activation
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

float FLUDD::calcRocketTimerPercent()
{
    return sead::MathCalcCommon<float>::max(sead::MathCalcCommon<float>::min(chargeTimer / 100.f, 1.f), 0.05f);
}

void FLUDD::setRefs() {
    al::LiveActor* hack = mario->mHackKeeper->currentHackActor;
    if (hack) {
        isHack = true;
    } else {
        isHack = false;
    }
    is2D = mario->mDimKeeper->is2D;
    isPGrounded = rs::isPlayerOnGround(mario);
    layout = stageSceneRef->stageSceneLayout->mCoinCountLyt;
    isPUnderWater = rs::isPlayerInWater(mario);
    isPInWater = al::isInWaterPos(mario, al::getTrans(mario));
}

void FLUDD::firstTimeSetup() {
    base->activate(mario);
    setFluddModeValues();
    resetLayout();
    hover->activate(base, false);
    rocket->activate(base, false);
    turbo->activate(base, false);
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
            if (!isPGrounded && !isPUnderWater) {
                return true;
            }
        }
        if (fluddMode == 1)  // ROCKET
        {
            jetOne->deactivate();
            jetTwo->deactivate();
            turbo->deactivate();
            hover->deactivate();
            if (!isPUnderWater && !isPGrounded) {
                return true;
            }
        }
        if (fluddMode == 2)  // TURBO
        {
            jetOne->deactivate();
            jetTwo->deactivate();
            rocket->deactivate();
            hover->deactivate();
            if (mario->mPlayerInput->isMove()) {  // checks if mario is moving
                    return true;
            }
        }
        
       
    }
    else if (isTankEmpty(tankStopValue)) {
        recharging = true;
    }

    return false;



    /* if (!al::isPadHoldR(-1)) {  // this should be first check made before activation
        return false;
    }

        if (isHack || PlayerFunction::isPlayerDeadStatus(mario) || rs::isActiveDemo(mario) ||
        isTankEmpty(tankStopValue) || !recharging) {  // these checks apply to all fludd modes
            return false;
        }

        if (isTankEmpty(tankStopValue)) {
            recharging = true;
        }

        switch (fluddMode) {
        case 0:  // HOVER
        {
            if (!isPGrounded && !isPUnderWater) {
                canActivate = true;
            }
        } break;
        case 1:  // ROCKET
        {
            if (!isPGrounded && !isPUnderWater) {
                canActivate = true;
            }
        } break;
        case 2:  // TURBO
        {
            if (mario->mPlayerInput->isMove()) {  // checks if mario is moving
                if (isPGrounded || isPInWater) {
                    canActivate = true;
                }
            }
        } break;
        default:
            canActivate = false;
            break;
        }*/

}
