//�O���[�o��
cbuffer global
{
	matrix g_w;//���[���h�s��
	matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ��s��
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
	float2 uv : TEXCOORD3;
};
//�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

    output.pos = mul(pos,g_w);
	output.uv = uv;

	return output;
}
//�s�N�Z���V�F�[�_�[
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 texel = g_Tex.Sample(g_Sampler, input.uv);
    float rgbTotal = texel.x + texel.y + texel.z;
    if (rgbTotal >= 2.99f)  // 2.99�ȏ�(�قڔ��Ȃ�)
    {
        discard; // �����ɂ����Ⴄ
    }
	
    return texel;
}