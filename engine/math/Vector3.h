#pragma once
struct Vector3 final {
	float x;
	float y;
	float z;
};

// 加算
Vector3 Add(const Vector3& pos, const Vector3& vector);
// 減算
Vector3 Subtract(const Vector3& pos1, const Vector3& pos2);
// 積
Vector3 Multiply(const Vector3& v1, const Vector3& v2);
// スカラー倍
Vector3 Multiply(const float& scalar, const Vector3& v);

// 内積
float Dot(const Vector3& v1, const Vector3& v2);
// 長さ(ノルム)
float Length(const Vector3& v);

// 正規化
Vector3 Normalize(const Vector3& v);