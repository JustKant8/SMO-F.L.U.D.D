#include <stdint.h>
#include "container/seadSafeArray.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerModelHolder.h"
#include "game/StageScene/StageScene.h"
#include "gfx/seadColor.h"
#include "prim/seadSafeString.h"
#include "sead/math/seadVector.h"
#include "al/LiveActor/LiveActor.h"
#include "game/Player/PlayerFunction.h"
#include "game/Player/PlayerJointControlPartsDynamics.h"
#include "game/Player/PlayerConst.h"
#include "actors/FluddBase.hpp"
#include "actors/FluddTurbo.hpp"
#include "actors/FluddHover.hpp"
#include "actors/FluddRocket.hpp"
#include "actors/HoverWater.hpp"


class FLUDD{
private:
    //Fludd settings/values
    int fluddMode = 0;    // Modes: 0 = hover, 1 = rocket, 2 = turbo
    float tank = 100.0f; //0 = empty, 100 = full
    float fluddRecharge = 1.0f;
    
    float fluddVel = 2.8f; //start off as hover velocity
    float chargeTimer = 0.0f; //replaces oxygen meter
    float chargeTimerDecrease = 1.5f;
    float tankStopValue = 0.0f;
    float tankRunoutVal = 33.3f;
    bool recharging = false;
    int turboWaterDelay = 0;

    //Layout values
    sead::Vector2f tankWaterTrans = sead::Vector2f::zero;
    sead::Vector2f tankWaterScale = sead::Vector2f::zero;
    sead::Vector2f tankStopLevelPos = sead::Vector2f::zero;
    sead::Vector2f chargeBarTrans = sead::Vector2f::zero;
    sead::Vector2f chargeBarScale = sead::Vector2f::zero;

    
    void activateFludd();
    void deactivateFludd();
    float smoothVelocity(float from, float to, float g);//smooth velocity transition
    float stopTankValue();
    void updateTankWaterLyt();

    //random
    bool tStopValueSet = false;
    const sead::SafeString action = "Fall";
    
    
public:
    //references
    StageScene* stageSceneRef;
    bool is2D;
    bool isHack;
    PlayerActorHakoniwa* mario;
    CoinCounter* layout;

    bool isMario = false;
    bool lJCancel = false;

    //fludd parts
    ca::FluddBase* base;
    ca::FluddTurbo* turbo;
    ca::FluddHover* hover;
    ca::FluddRocket* rocket;
    ca::HoverWater* jetOne;
    ca::HoverWater* jetTwo;

    bool isPUnderWater = false;
    bool isPInWater = false;
    bool isPGrounded = false;
    bool stageChange = false;
    bool doOnce = false;
    bool setNrvGrounded = false;
    float fluddDischarge = 0.1f;

    bool active = false;


    //update
    void updateFludd(); //Main loop run every frame

    //Getters
    int getFluddMode();

    //Is
    bool isTankEmpty(float f1); //Checks if tank is empty or not
    bool isChargeTimerFull();
    bool canFluddActivate(); //does all fludd checks

    //Setters
    void fluddTankFill();
    void changeFluddModeR(); //right d-pad press
    void changeFluddModeL(); //left d-pad press
    void setFluddModeValues();
    void resetLayout();
    void setRefs();
    void initModels(al::ActorInitInfo const& info);
    void firstTimeSetup();

    //Calcs
    float calcRocketTimerPercent();
};

FLUDD& Fludd();