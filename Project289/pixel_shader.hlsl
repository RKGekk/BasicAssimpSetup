cbuffer alphaBuffer : register(b0) {
	float alpha;
};

struct PS_INPUT {
	float4 pos : SV_POSITION;
	float3 color : COLOR;
	float2 uv : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET {
	float3 pixelColor = objTexture.Sample(objSamplerState, input.uv);
	return float4(pixelColor, alpha);
	//return float4(pixelColor, 1.0f);
	//return float4(input.color, 1.0f);
}