//�O���[�o��
cbuffer global
{
	matrix g_w;//���[���h�s��
	matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ��s��
    matrix g_wp;
	matrix g_wlp; 
	matrix g_wlpt;
	float4 g_lightPos;//���C�g�̕����x�N�g��
	float4 g_eye;	//�J�����i���_�j
	float4 g_ambient;//����
	float4 g_diffuse;//�g�U���ˌ�
	float4 g_specular;//���ʔ��ˌ�
};

Texture2D g_Tex: register(t0);
SamplerState g_Sampler : register(s0);

//�\����
struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 light : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 eyeVector : TEXCOORD2;
	float2 uv : TEXCOORD3;
};
//�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = mul(pos, g_wvp);
	output.normal = abs(mul(normal, (float3x3)g_w));
    float4 a = (1.0f, 0.0f, 0.0f, 1.0f);
    output.light = a; //g_lightPos - pos;
	float3 posWorld = mul(pos, g_w);
	output.eyeVector = g_eye - posWorld;  //���̂��王�_�ւ̕����x�N�g��
	output.uv = uv;

	return output;
}
//�s�N�Z���V�F�[�_�[
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float3 normal = normalize(input.normal);      // �@���𐳋K��
	float3 lightDir = normalize(input.light);     // ���C�g�x�N�g���̐��K��
	float3 viewDir = normalize(input.eyeVector);  // �����x�N�g���̐��K��
	float4 nl = saturate(dot(normal, lightDir));  // ���K������Ă�̂�  nl = cos(�@���ƌ��̂Ȃ��p)

	float3 reflect = normalize(2 * nl * normal - lightDir);   // ���˃x�N�g��
	//                ���ʔ��ˌ� * cons(���˃x�N�g���Ɩڐ��ւ̃x�N�g��)�O�P�x
	float4 specular = g_specular * pow(saturate(dot(reflect, viewDir)),2); // 2�͋P�x(�ǂ̂��炢�s�J���Ƃ��邩�A������΍����قǃs�J��)

	float4 texel = g_Tex.Sample(g_Sampler, input.uv);
	// �g�U���ˌ�
    float4 diffuse = (g_diffuse + texel) * nl;
	// �g�U���ˌ����� ���ʔ��˂������ďo���オ��
    float4 color = diffuse + specular;
	float4 final = color;
	final.w = 1.0f;
	return final;
}