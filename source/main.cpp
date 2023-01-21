#include "main.hpp"
#include "al/area/AreaObj.h"
#include "al/area/AreaObjGroup.h"
#include "al/camera/CameraDirector.h"
#include "al/camera/CameraPoser.h"
#include "al/util.hpp"
#include "al/util/LiveActorUtil.h"
#include "al/util/StringUtil.h"
#include "cameras/CameraPoserCustom.h"
#include "debugMenu.hpp"

#include <string>
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerFunction.h"
#include "game/Player/PlayerInput.h"
#include "game/Player/PlayerOxygen.h"
#include "fludd.h"

static bool showMenu = false;

static bool isInGame = false;

DebugWarpPoint warpPoints[40];

int listCount = 0;

int curWarpPoint = 0;

void drawBackground(agl::DrawContext *context)
{

    sead::Vector3<float> p1; // top left
    p1.x = -1.0;
    p1.y = 0.3;
    p1.z = 0.0;
    sead::Vector3<float> p2; // top right
    p2.x = -0.2;
    p2.y = 0.3;
    p2.z = 0.0;
    sead::Vector3<float> p3; // bottom left
    p3.x = -1.0;
    p3.y = -1.0;
    p3.z = 0.0;
    sead::Vector3<float> p4; // bottom right
    p4.x = -0.2;
    p4.y = -1.0;
    p4.z = 0.0;

    sead::Color4f c;
    c.r = 0.1;
    c.g = 0.1;
    c.b = 0.1;
    c.a = 0.9;

    agl::utl::DevTools::beginDrawImm(context, sead::Matrix34<float>::ident, sead::Matrix44<float>::ident);
    agl::utl::DevTools::drawTriangleImm(context, p1, p2, p3, c);
    agl::utl::DevTools::drawTriangleImm(context, p3, p4, p2, c);
}

// ------------- Hooks -------------

al::StageInfo *initDebugListHook(const al::Scene *curScene)
{

    // hook that gets all objects put in DebugList and adds their coordinates to a warp point array

    al::StageInfo *info = al::getStageInfoMap(curScene, 0);

    al::PlacementInfo rootInfo = al::PlacementInfo();

    al::tryGetPlacementInfoAndCount(&rootInfo, &listCount, info, "DebugList");

    if (listCount > 0)
    {
        for (size_t i = 0; i < listCount; i++)
        {
            al::PlacementInfo objInfo = al::PlacementInfo();

            al::getPlacementInfoByIndex(&objInfo, rootInfo, i);

            const char *displayName = "";
            al::tryGetDisplayName(&displayName, objInfo);

            strcpy(warpPoints[i].pointName, displayName);

            al::tryGetTrans(&warpPoints[i].warpPos, objInfo);
        }
    }

    return info;
}

void drawMainHook(HakoniwaSequence *curSequence, sead::Viewport *viewport, sead::DrawContext *drawContext)
{

    if (!showMenu)
    {
        al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
        return;
    }

    StageScene* stageScene = (StageScene*)curSequence->curScene;

    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* p1 = al::tryGetPlayerActor(pHolder, 0);

    sead::Vector3f* b = getTransPtr(Fludd().base);
    sead::Vector3f* d = rs::getDemoPlayerTrans(p1);

    int dispWidth = al::getLayoutDisplayWidth();
    int dispHeight = al::getLayoutDisplayHeight();

    gTextWriter->mViewport = viewport;

    gTextWriter->mColor = sead::Color4f(
        1.f,
        1.f,
        1.f,
        0.8f);

    al::Scene *curScene = curSequence->curScene;

    if (curScene && isInGame)
    {

        drawBackground((agl::DrawContext *)drawContext);

        gTextWriter->beginDraw();

        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (dispHeight / 3) + 30.f));

        gTextWriter->setScaleFromFontHeight(20.f);

        if (p1) {
            
                gTextWriter->printf("_______Fludd Controls_______\n");
                gTextWriter->printf("Hover: Hold (R) - while in the air\n");
                gTextWriter->printf("Rocket: Hold (R) - press (A) when charged\n");
                gTextWriter->printf("Turbo: Hold (R) - move in direction\n" );
                gTextWriter->printf("- in water hold A(move up), hold B(move down)\n");
                gTextWriter->printf("Enable/disable debug: Hold(L), press up on D-Pad\n");

                gTextWriter->printf("\nEnable Left Stick(mode change): \n");
                gTextWriter->printf("Hold (L), press down left stick: %s\n", BTOC(Fludd().stickActive));

                gTextWriter->printf("-------Fludd Connectors-------\n");
                gTextWriter->printf("Fludd Base Connecting: %s \n", BTOC(Fludd().base->isConnecting));
                gTextWriter->printf("Fludd Hover Connecting: %s \n", BTOC(Fludd().hover->isConnecting));
                gTextWriter->printf("Fludd Rocket Connecting: %s \n", BTOC(Fludd().rocket->isConnecting));
                gTextWriter->printf("Fludd Turbo Connecting: %s \n", BTOC(Fludd().turbo->isConnecting));


                gTextWriter->printf("Current Fludd mode: %i \n", Fludd().getFluddMode());
                gTextWriter->printf("Fludd Base Position: %f, %f, %f \n", b->x, b->y, b->z);
                gTextWriter->printf("Is stage changed: %s \n", BTOC(Fludd().stageChange));

                gTextWriter->printf("Mario Model: %s \n", Fludd().marioModel->mActorName);
                gTextWriter->printf("Is in demo with player: %s \n", BTOC(rs::isActiveDemo(p1)));
                gTextWriter->printf("Demo mario Position: %f, %f, %f \n", d->x, d->y, d->z);
        }

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
}

void sceneInitHook(StageScene* scene, const al::ActorInitInfo& initInfo, char* listName) {
    al::initPlacementObjectMap(scene, initInfo, listName);

    Fludd().stageChange = true;
    
}

void stageInitHook(al::ActorInitInfo* info, StageScene* curScene,
                   al::PlacementInfo const* placement, al::LayoutInitInfo const* lytInfo,
                   al::ActorFactory const* factory, al::SceneMsgCtrl* sceneMsgCtrl,
                   al::GameDataHolderBase* dataHolder) {
    al::initActorInitInfo(info, curScene, placement, lytInfo, factory, sceneMsgCtrl, dataHolder);


    Fludd().initModels(*info);

    Fludd().stageChange = true;
}

bool sceneKillHook(GameDataHolderAccessor value) {
    Fludd().fluddTankFill();
    Fludd().layout = nullptr;
    Fludd().stageChange = true;

    return GameDataFunction::isMissEndPrevStageForSceneDead(value);
}


ulong threadInit()
{ // hook for initializing any threads we need
    __asm("STR X21, [X19,#0x208]");

    return 0x20;
}

bool triggerR(int port) {
    return false;
}

bool hakoniwaSequenceHook(HakoniwaSequence* sequence) {
    StageScene* stageScene = (StageScene*)sequence->curScene;

    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* p1 = al::tryGetPlayerActor(pHolder, 0);//tryGetPlayerActor pHolder, 0

    bool isFirstStep = al::isFirstStep(sequence);

    if (al::isPadTriggerUp(-1) && al::isPadHoldL(-1))  // enables/disables debug menu
    {
        showMenu = !showMenu;
    }

    
    Fludd().stageSceneRef = stageScene;
    Fludd().mario = p1;

    isInGame = !stageScene->isPause();

    // Fludd code
    if (!stageScene->isPause())
        Fludd().updateFludd();

    return isFirstStep;
}

void seadPrintHook(const char *fmt, ...) // hook for replacing sead::system::print with our custom logger
{
    va_list args;
    va_start(args, fmt);

    gLogger->LOG(fmt, args);

    va_end(args);
}