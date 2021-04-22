#pragma once

#include <DirectXMath.h>

struct CB_VS_VertexShader {
	DirectX::XMFLOAT4X4 mat;
};

struct CB_PS_PixelShader {
	float alpha = 1.0f;
};