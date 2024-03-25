cbuffer global : register(b0)
{
	matrix g_wvp;
};

Texture2D g_tex: register(t0);
SamplerState g_sampler : register(s0);

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 uv : TEXCOORDO;
};

//
//バーテックスシェーダー
//
VS_OUTPUT VS(float4 Pos : POSITION, float4 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(Pos, g_wvp);
	output.uv = uv;

	return output;
}
//
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = g_tex.Sample(g_sampler, input.uv);

	texel.w = 0.5f;
	return texel;
}
