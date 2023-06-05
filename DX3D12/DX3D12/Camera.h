#pragma once
#include "MathHelper.h"

using namespace DirectX;
class Camera
{
public:
    Camera();
    ~Camera();

    XMVECTOR GetPosition() const;
    XMFLOAT3 GetPosition3f() const;
    void SetPosition(float x,float y,float z);
    void SetPosition(const XMFLOAT3& v);

    XMVECTOR GetRight() const;
    XMFLOAT3 GetRight3f() const;
    XMVECTOR GetUp() const;
    XMFLOAT3 GetUp3f() const;
    XMVECTOR GetLook() const;
    XMFLOAT3 GetLook3f() const;

    float GetNearZ() const;
    float GetFarZ() const;
    float GetAspect() const;
    float GetFovY() const;
    float GetFovX() const;

    float GetNearWindowWidth() const;
    float GetNearWindowHeight() const;
    float GetFarWindowWidth() const;
    float GetFarWindowHeight() const;

    void SetLens(float fovY,float aspect,float zn,float zf);

    void LookAt(FXMVECTOR pos,FXMVECTOR target,FXMVECTOR worldUp);
    void LookAt(const XMFLOAT3& pos,const XMFLOAT3& target,const XMFLOAT3& up);

    XMMATRIX GetView() const;
    XMMATRIX GetProj() const;

    XMFLOAT4X4 GetView4x4f() const;
    XMFLOAT4X4 GetProj4x4f() const;

    void Strafe(float d);
    void Walk(float d);
    void Pitch(float angle);
    void RotateY(float angle);

    void UpdateViewMatrix();
private:
    XMFLOAT3 mPosition = {0.0f,0.0f,0.0f};
    XMFLOAT3 mRight = {1.0f,0.0f,0.0f};
    XMFLOAT3 mUp = {0.0f,1.0f,0.0f};
    XMFLOAT3 mLook = {0.0f,0.0f,1.0f};

    float mNearZ = 0.0f;
    float mFarZ = 0.0f;
    float mAspect = 0.0f;
    float mFovY = 0.0f;
    float mNearWindowHeight = 0.0f;
    float mFarWindowHeight = 0.0f;

    bool mViewDirty = true;

    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();
};
