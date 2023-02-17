#include "cameras/CameraPoserCustom.h"
#include <math.h>
#include "al/camera/CameraAngleVerticalCtrl.h"
#include "al/camera/CameraPoser.h"
#include "al/camera/alCameraPoserFunction.h"
#include "al/util/MathUtil.h"
#include "al/util/VectorUtil.h"
#include "logger.hpp"
#include "sead/gfx/seadCamera.h"
#include "sead/math/seadVector.h"
#include "sead/math/seadVectorCalcCommon.h"

namespace cc {

// Most of the code here is the reversed code for CameraPoserFollowSimple (unused), but i've
// modified it to allow vertical angle changes as well as horizontal.

CameraPoserCustom::CameraPoserCustom(const char* poserName) : CameraPoser(poserName) {
    this->initOrthoProjectionParam();
}

void CameraPoserCustom::init(void) {
    alCameraPoserFunction::initSnapShotCameraCtrlZoomRollMove(
        this);  // this makes the snapshot camera have the abilities of the normal snapshot cam, but
                // is locked rotationally
    alCameraPoserFunction::initCameraVerticalAbsorber(this);
    alCameraPoserFunction::initCameraAngleCtrl(this);
}

void CameraPoserCustom::start(al::CameraStartInfo const&) {
    sead::Vector3f faceDir;
    sead::Vector3f targetFront;


    if (alCameraPoserFunction::isSceneCameraFirstCalc(this)) {
        alCameraPoserFunction::calcTargetTrans(&mTargetTrans, this);
        mTargetTrans.y += mOffsetY;
        targetFront = sead::Vector3f(0, 0, 0);
        alCameraPoserFunction::calcTargetFront(&targetFront, this);

        faceDir = mTargetTrans - (mDist * targetFront);

    } else {
        sead::LookAtCamera* curLookCam = alCameraPoserFunction::getLookAtCamera(this);

        sead::Vector3f curPos = curLookCam->getPos();

        sead::Vector3f curAt = curLookCam->getAt();

        targetFront = sead::Vector3f(curPos.x - curAt.x, 0.0, curPos.z - curAt.z);

        al::tryNormalizeOrDirZ(&targetFront);

        faceDir = (mDist * targetFront) + mTargetTrans;
    }

    mPosition = faceDir;

}

void CameraPoserCustom::movement(void) {
    sead::Vector3f targetDir;

    mTargetTrans.y += mOffsetY;

    targetDir = sead::Vector3f(mPosition.x - mTargetTrans.x, 0.0f, mPosition.z - mTargetTrans.z);

    al::tryNormalizeOrDirZ(&targetDir);

    sead::Vector2f playerInput(0, 0);
    alCameraPoserFunction::calcCameraRotateStick(&playerInput, this);

     //al::rotateVectorDegreeY(&targetDir, (playerInput.x > 0.0f ? playerInput.x : -playerInput.x) < 0.02f ?
     //                           0.0f : playerInput.x * -2.0f);

     mAngle += playerInput.y * -1.0f;
     mAngle = al::clamp(mAngle, -50.0f, 50.0f);

     sead::Vector3f rotatedVec = targetDir;
    
    sead::Vector3f rightAxis;
    rightAxis.setCross(targetDir, mCameraUp);

    float stickSpeed = alCameraPoserFunction::getStickSensitivityScale(this) *
                       alCameraPoserFunction::getStickSensitivityLevel(this);

    

    // Horizontal Rotation
    al::rotateVectorDegree(&rotatedVec, rotatedVec, mCameraUp, playerInput.x * -stickSpeed);

    // Vertical Rotation
    al::rotateVectorDegree(&rotatedVec, rotatedVec, rightAxis, mAngle);

    sead::Vector3f rightVec = rotatedVec;

    al::rotateVectorDegreeY(&rightVec, 90.0f);

    mPosition = mTargetTrans + (rotatedVec * mDist);

    
}
}  // namespace cc