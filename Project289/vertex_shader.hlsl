cbuffer myCbuffer : register(b0) {
	float4x4 mat;
};

struct VS_INPUT {
	float3 pos : POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
};

struct VS_OUTPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.pos = mul(float4(input.pos, 1.0f), mat);
	output.color = input.color;
	output.uv = input.uv;
	return output;
}