#include "Common.hlsli"

struct VSInput {
	float3	position : POSITION;
	float2	uv : TEXCOORD;
	float3	normal : NORMAL;
	uint4	bone_indices : BONE_INDICES;
	float4	bone_weights : BONE_WEIGHTS;
	float4  offset : VERTEX_OFFSET;
};

struct PSInput {
	float4 position : SV_Position;
	float3 world_position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

PSInput MainVS(VSInput input) {
	PSInput output = (PSInput)0;
	output.position = float4(input.position + input.offset.xyz, 1.f);

	float4x4 comb = (float4x4)0;
	for (int i = 0; i < 4; ++i) {
		comb += input.bone_weights[i] * World[input.bone_indices[i]];
	}
	output.position = mul(output.position, comb);
	output.world_position = output.position.xyz;

	output.position = mul(output.position, WorldToScreen);

	MaterialParameter material = MaterialParameters[MaterialIndex];
	output.color = material.DiffuseColor;
	output.uv = input.uv;
	output.normal = mul(input.normal, (float3x3)comb);

	return output;
}

float4 MainPS(PSInput input) : SV_TARGET{
	float4 output = ColorTex.Sample(DefaultSampler, input.uv);
	output *= input.color;

	return output;
}
