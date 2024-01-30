#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"

// 線形補間
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

// 球面線形補間
Vector3 Slerp(Vector3 start, Vector3 end, float t);