cbuffer SceneConstantValues : register(b0) {
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldToScreen; // View * Projection
	float3 EyePosition;
	float3 EyeForward;
	float3 LightDirection;
	float4 ShadowColor;
}

cbuffer ModelConstantValues : register(b1) {
	float4x4 World[1024];
}

struct MaterialParameter {
	float4 DiffuseColor;
};

cbuffer MaterialConstantValues : register(b2) {
	MaterialParameter MaterialParameters[256];
}

cbuffer MaterialConstant : register(b3) {
	uint MaterialIndex;
}

Texture2D<float4> ColorTex : register(t0);

SamplerState DefaultSampler : register(s0);