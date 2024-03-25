cbuffer global: register(b0)
{
    matrix g_w; //���[���h�s��
    matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ������s��
    matrix g_wp; // ���[���h * �v���W�F�N�V���� 
    matrix g_wlp; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍��� �� ���_�����C�g�̈ʒu�Ƃ����Ƃ��̑��ΓI�Ȉʒu���W
    matrix g_wlpt; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV�s��̍���
    float4 g_lightpos; //�|�C���g���C�g
    float4 g_eye; //�J�����ʒu
    float4 g_ambient; //����
    float4 g_diffuse; //�g�U���ˌ�
    float4 g_specular; //���ʔ��ˌ�

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
	// rbg�l�̑��a�� 3.0f �܂蔒�F�Ȃ瓧���ɂ���
	if (rgbTotal >= 3.f) {
        discard; // �����Ȃ�̂Ă��Ⴄ
    }
	return texel;
}