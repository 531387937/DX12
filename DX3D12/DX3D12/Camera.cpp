#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPosition() const
{
    return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition3f() const
{
    return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
    mPosition = XMFLOAT3(x,y,z);
}

void Camera::SetPosition(const XMFLOAT3& v)
{
    mPosition = v;
}

XMVECTOR Camera::GetRight() const
{
}

XMFLOAT3 Camera::GetRight3f() const
{
}

XMVECTOR Camera::GetUp() const
{
}

XMFLOAT3 Camera::GetUp3f() const
{
}

XMVECTOR Camera::GetLook() const
{
}

XMFLOAT3 Camera::GetLook3f() const
{
}

float Camera::GetNearZ() const
{
}

float Camera::GetFarZ() const
{
}

float Camera::GetAspect() const
{
}

float Camera::GetFovY() const
{
}

float Camera::GetFovX() const
{
}

float Camera::GetNearWindowWidth() const
{
}

float Camera::GetNearWindowHeight() const
{
}

float Camera::GetFarWindowWidth() const
{
}

float Camera::GetFarWindowHeight() const
{
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
    mFovY = fovY;
    mAspect = aspect;
    mNearZ = zn;
    mFarZ = zf;

    mNearWindowHeight = 2.0f*mNearZ*tanf(0.5f*mFovY);
    mFarWindowHeight = 2.0f*mFarZ*tanf(0.5f*mFovY);

    XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY,mAspect,mNearZ,mFarZ);
    XMStoreFloat4x4(&mProj,P);
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
}

XMMATRIX Camera::GetView() const
{
}

XMMATRIX Camera::GetProj() const
{
}

XMFLOAT4X4 Camera::GetView4x4f() const
{
}

XMFLOAT4X4 Camera::GetProj4x4f() const
{
}

void Camera::Strafe(float d)
{
}

void Camera::Walk(float d)
{
}

void Camera::Pitch(float angle)
{
}

void Camera::RotateY(float angle)
{
}

void Camera::UpdateViewMatrix()
{
}
