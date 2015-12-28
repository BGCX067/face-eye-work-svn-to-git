#ifndef __DirectX_Math_Util_h__
#define __DirectX_Math_Util_h__

#include <DirectXMath.h>
#define PI 3.14159265359
namespace DirectX
{

struct MXMFLOAT2 : public XMFLOAT2
{
	MXMFLOAT2() {}
	MXMFLOAT2(float _x, float _y) : XMFLOAT2(_x, _y) {}
	explicit MXMFLOAT2(_In_reads_(2) const float *pArray) : XMFLOAT2(pArray) {}
	operator XMFLOAT2()
	{
		return *((XMFLOAT2*)this);
	}

	MXMFLOAT2 operator+(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a + _b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator+=(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a + _b;
		XMStoreFloat2(this, _c);
		return *this;
	}

	MXMFLOAT2 operator-(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a - _b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator-=(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a - _b;
		XMStoreFloat2(this, _c);
		return *this;
	}

	MXMFLOAT2 operator*(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a * _b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator*=(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a * _b;
		XMStoreFloat2(this, _c);
		return *this;
	}

	MXMFLOAT2 operator*(float b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _c = _a * b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator*=(float b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _c = _a * b;
		XMStoreFloat2(this, _c);
		return *this;
	}

	MXMFLOAT2 operator/(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a / _b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator/=(XMFLOAT2 b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _b = XMLoadFloat2(&b);
		XMVECTOR _c = _a / _b;
		XMStoreFloat2(this, _c);
		return *this;
	}

	MXMFLOAT2 operator/(float b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _c = _a / b;
		MXMFLOAT2 res;
		XMStoreFloat2(&res, _c);
		return res;
	}

	MXMFLOAT2 operator/=(float b)
	{
		XMVECTOR _a = XMLoadFloat2(this);
		XMVECTOR _c = _a / b;
		XMStoreFloat2(this, _c);
		return *this;
	}
};


struct MXMFLOAT3 : public XMFLOAT3
{
	MXMFLOAT3() {}
	MXMFLOAT3(float _x, float _y, float _z) : XMFLOAT3(_x, _y, _z) {}
	explicit MXMFLOAT3(_In_reads_(2) const float *pArray) : XMFLOAT3(pArray) {}

	operator XMFLOAT3()
	{
		return *((XMFLOAT3*)this);
	}

	MXMFLOAT3 operator+(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a + _b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator+=(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a + _b;
		XMStoreFloat3(this, _c);
		return *this;
	}

	MXMFLOAT3 operator-(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a - _b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator-=(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a - _b;
		XMStoreFloat3(this, _c);
		return *this;
	}

	MXMFLOAT3 operator*(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a * _b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator*=(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a * _b;
		XMStoreFloat3(this, _c);
		return *this;
	}

	MXMFLOAT3 operator*(float b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _c = _a * b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator*=(float b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _c = _a * b;
		XMStoreFloat3(this, _c);
		return *this;
	}

	MXMFLOAT3 operator/(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a / _b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator/=(XMFLOAT3 b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _b = XMLoadFloat3(&b);
		XMVECTOR _c = _a / _b;
		XMStoreFloat3(this, _c);
		return *this;
	}

	MXMFLOAT3 operator/(float b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _c = _a / b;
		MXMFLOAT3 res;
		XMStoreFloat3(&res, _c);
		return res;
	}

	MXMFLOAT3 operator/=(float b)
	{
		XMVECTOR _a = XMLoadFloat3(this);
		XMVECTOR _c = _a / b;
		XMStoreFloat3(this, _c);
		return *this;
	}
};


struct MXMFLOAT4 : public XMFLOAT4
{
	MXMFLOAT4() {}
	MXMFLOAT4(float _x, float _y, float _z, float _w) : XMFLOAT4(_x, _y, _z, _w) {}
	explicit MXMFLOAT4(_In_reads_(2) const float *pArray) : XMFLOAT4(pArray) {}
	operator XMFLOAT4()
	{
		return *((XMFLOAT4*)this);
	}

	MXMFLOAT4 operator+(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a + _b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator+=(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a + _b;
		XMStoreFloat4(this, _c);
		return *this;
	}

	MXMFLOAT4 operator-(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a - _b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator-=(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a - _b;
		XMStoreFloat4(this, _c);
		return *this;
	}

	MXMFLOAT4 operator*(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a * _b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator*=(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a * _b;
		XMStoreFloat4(this, _c);
		return *this;
	}

	MXMFLOAT4 operator*(float b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _c = _a * b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator*=(float b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _c = _a * b;
		XMStoreFloat4(this, _c);
		return *this;
	}

	MXMFLOAT4 operator/(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a / _b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator/=(XMFLOAT4 b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _b = XMLoadFloat4(&b);
		XMVECTOR _c = _a / _b;
		XMStoreFloat4(this, _c);
		return *this;
	}

	MXMFLOAT4 operator/(float b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _c = _a / b;
		MXMFLOAT4 res;
		XMStoreFloat4(&res, _c);
		return res;
	}

	MXMFLOAT4 operator/=(float b)
	{
		XMVECTOR _a = XMLoadFloat4(this);
		XMVECTOR _c = _a / b;
		XMStoreFloat4(this, _c);
		return *this;
	}
};


}; // namespace DirectX

#endif
