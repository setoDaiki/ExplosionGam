cbuffer global: register(b0)
{
    matrix g_w; //ワールド行列
    matrix g_wvp; //ワールドから射影までの変換合成行列
    matrix g_wp; // ワールド * プロジェクション 
    matrix g_wlp; //ワールド・”ライトビュー”・プロジェクションの合成 → 視点がライトの位置としたときの相対的な位置座標
    matrix g_wlpt; //ワールド・”ライトビュー”・プロジェクション・UV行列の合成
    float4 g_lightpos; //ポイントライト
    float4 g_eye; //カメラ位置
    float4 g_ambient; //環境光
    float4 g_diffuse; //拡散反射光
    float4 g_specular; //鏡面反射光

};
Texture2D g_tex: register(t0);
SamplerState g_sampler : register(s0);

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

//
//
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(pos, g_wvp);
	output.uv = tex;

	return output;
}
//
//
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = g_tex.Sample(g_sampler, input.uv);
	float rgbTotal = texel.x + texel.y + texel.z;
	// rbg値の総和が 3.0f つまり白色なら透明にする
	if (rgbTotal >= 3.f) {
        discard; // 白いなら捨てちゃう
    }
	return texel;
}