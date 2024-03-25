texture2D g_tex: register(t0);
texture2D g_shadowMap : register(t1);//シャドウマップテクスチャー 
SamplerState g_sampler : register(s0);

cbuffer global
{
	matrix g_w;//ワールド行列
	matrix g_wvp; //ワールドから射影までの変換合成行列
    matrix g_wp;
	matrix g_wlp; //ワールド・”ライトビュー”・プロジェクションの合成 → 視点がライトの位置としたときの相対的な位置座標
	matrix g_wlpt; //ワールド・”ライトビュー”・プロジェクション・UV行列の合成
	float4 g_lightpos;//ポイントライト
	float4 g_eye;//カメラ位置
	float4 g_ambient;//環境光
	float4 g_diffuse; //拡散反射光
	float4 g_specular;//鏡面反射光
};

//バーテックスシェーダー出力構造体
struct VS_OUTPUT_DEPTH//シャドウマップへレンダー用
{
	float4 pos : SV_POSITION;
	float4 depth :POSITION;
};

struct VS_OUTPUT//通常レンダー用
{
	float4 pos : SV_POSITION;
	float3 light : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 eyeVector : TEXCOORD2;
	float4 posWorld : TEXCOORD3;
	float2 tex : TEXCOORD4;
	float4 lighttexCoord : TEXCOORD5;
	float4 lightViewpos : TEXCOORD6;
};
//
//シャドウマップ書き込み用バーテックスシェーダー
//
VS_OUTPUT_DEPTH VS_Write(float4 pos : POSITION)
{
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;
	output.pos = mul(pos, g_wlp);
	output.depth = output.pos;

	return output;
}
//
//シャドウマップ書き込み用ピクセルシェーダー
//
float4 PS_Write(VS_OUTPUT_DEPTH input) : SV_Target
{
	float4 color = input.depth.z / input.depth.w;
	return color;
}
//
//バーテックスシェーダー
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//射影変換（ワールド→ビュー→プロジェクション）
	output.pos = mul(pos, g_wvp);
	//法線をモデルの姿勢に合わせる(モデルが回転すれば法線も回転させる必要があるため）
	output.normal = abs(mul(normal, (float3x3)g_w));
	//ライト方向
	output.light = normalize(g_lightpos - pos);
	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.posWorld = mul(pos, g_w);
	output.eyeVector = normalize(g_eye - output.posWorld);
	//テクスチャー座標
	output.tex = tex;
	//ライトビューを参照するとき、手がかりとなる[テクスチャー座標](0~1の範囲ということ?)
	output.lighttexCoord = mul(pos, g_wlpt);//この点が、ライトビューであったときの位置がわかる
	//ライトビューにおける位置(変換後)
	output.lightViewpos = mul(pos, g_wlp);

	return output;
}

//
//ピクセルシェーダー
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	//環境光　項
	float4 ambient = g_ambient;
	//拡散反射光　項
	float NL = saturate(dot(input.normal, input.light));
	/*float4 diffuse = (g_diffuse / 2 + g_tex.Sample(g_sampler, input.tex) / 2)*NL;
	//鏡面反射光　項
	float3 reflect = normalize(2 * NL*input.normal - input.light);
	float4 specular = pow(saturate(dot(reflect,input.eyeVector)), 4)*g_specular;
	//フォンモデル最終色　３つの項の合計
	float4 color = ambient + diffuse + specular;*/
    float3 reflect = normalize(2 * NL * input.normal - input.light); // 反射ベクトル
	//                鏡面反射光 * cons(反射ベクトルと目線へのベクトル)＾輝度
    float4 specular = g_specular * pow(saturate(dot(reflect, input.eyeVector)), 2); // 2は輝度(どのくらいピカっとするか、高ければ高いほどピカル)

    float4 texel = g_tex.Sample(g_sampler, input.tex);
	// 拡散反射光
    float4 diffuse = (g_diffuse + texel) * NL;
	// 拡散反射光完成 鏡面反射も足して出来上がり
    float4 color = diffuse + specular;
  
	//影の処理
	input.lighttexCoord /= input.lighttexCoord.w;
	float texValue = g_shadowMap.Sample(g_sampler,input.lighttexCoord).r;
	float lightLength = input.lightViewpos.z / input.lightViewpos.w;
	if (texValue + 0.0001 < lightLength)//ライトビューでの長さが短い（ライトビューでは遮蔽物がある）
	{
		color /= 3;//影
	}

	return color;
}