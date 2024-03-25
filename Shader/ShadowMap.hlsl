texture2D g_tex: register(t0);
texture2D g_shadowMap : register(t1);//�V���h�E�}�b�v�e�N�X�`���[ 
SamplerState g_sampler : register(s0);

cbuffer global
{
	matrix g_w;//���[���h�s��
	matrix g_wvp; //���[���h����ˉe�܂ł̕ϊ������s��
    matrix g_wp;
	matrix g_wlp; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����̍��� �� ���_�����C�g�̈ʒu�Ƃ����Ƃ��̑��ΓI�Ȉʒu���W
	matrix g_wlpt; //���[���h�E�h���C�g�r���[�h�E�v���W�F�N�V�����EUV�s��̍���
	float4 g_lightpos;//�|�C���g���C�g
	float4 g_eye;//�J�����ʒu
	float4 g_ambient;//����
	float4 g_diffuse; //�g�U���ˌ�
	float4 g_specular;//���ʔ��ˌ�
};

//�o�[�e�b�N�X�V�F�[�_�[�o�͍\����
struct VS_OUTPUT_DEPTH//�V���h�E�}�b�v�փ����_�[�p
{
	float4 pos : SV_POSITION;
	float4 depth :POSITION;
};

struct VS_OUTPUT//�ʏ탌���_�[�p
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
//�V���h�E�}�b�v�������ݗp�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT_DEPTH VS_Write(float4 pos : POSITION)
{
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;
	output.pos = mul(pos, g_wlp);
	output.depth = output.pos;

	return output;
}
//
//�V���h�E�}�b�v�������ݗp�s�N�Z���V�F�[�_�[
//
float4 PS_Write(VS_OUTPUT_DEPTH input) : SV_Target
{
	float4 color = input.depth.z / input.depth.w;
	return color;
}
//
//�o�[�e�b�N�X�V�F�[�_�[
//
VS_OUTPUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	//�ˉe�ϊ��i���[���h���r���[���v���W�F�N�V�����j
	output.pos = mul(pos, g_wvp);
	//�@�������f���̎p���ɍ��킹��(���f������]����Ζ@������]������K�v�����邽�߁j
	output.normal = abs(mul(normal, (float3x3)g_w));
	//���C�g����
	output.light = normalize(g_lightpos - pos);
	//�����x�N�g���@���[���h��ԏ�ł̒��_���王�_�֌������x�N�g��
	output.posWorld = mul(pos, g_w);
	output.eyeVector = normalize(g_eye - output.posWorld);
	//�e�N�X�`���[���W
	output.tex = tex;
	//���C�g�r���[���Q�Ƃ���Ƃ��A�肪����ƂȂ�[�e�N�X�`���[���W](0~1�͈̔͂Ƃ�������?)
	output.lighttexCoord = mul(pos, g_wlpt);//���̓_���A���C�g�r���[�ł������Ƃ��̈ʒu���킩��
	//���C�g�r���[�ɂ�����ʒu(�ϊ���)
	output.lightViewpos = mul(pos, g_wlp);

	return output;
}

//
//�s�N�Z���V�F�[�_�[
//
float4 PS(VS_OUTPUT input) : SV_Target
{
	//�����@��
	float4 ambient = g_ambient;
	//�g�U���ˌ��@��
	float NL = saturate(dot(input.normal, input.light));
	/*float4 diffuse = (g_diffuse / 2 + g_tex.Sample(g_sampler, input.tex) / 2)*NL;
	//���ʔ��ˌ��@��
	float3 reflect = normalize(2 * NL*input.normal - input.light);
	float4 specular = pow(saturate(dot(reflect,input.eyeVector)), 4)*g_specular;
	//�t�H�����f���ŏI�F�@�R�̍��̍��v
	float4 color = ambient + diffuse + specular;*/
    float3 reflect = normalize(2 * NL * input.normal - input.light); // ���˃x�N�g��
	//                ���ʔ��ˌ� * cons(���˃x�N�g���Ɩڐ��ւ̃x�N�g��)�O�P�x
    float4 specular = g_specular * pow(saturate(dot(reflect, input.eyeVector)), 2); // 2�͋P�x(�ǂ̂��炢�s�J���Ƃ��邩�A������΍����قǃs�J��)

    float4 texel = g_tex.Sample(g_sampler, input.tex);
	// �g�U���ˌ�
    float4 diffuse = (g_diffuse + texel) * NL;
	// �g�U���ˌ����� ���ʔ��˂������ďo���オ��
    float4 color = diffuse + specular;
  
	//�e�̏���
	input.lighttexCoord /= input.lighttexCoord.w;
	float texValue = g_shadowMap.Sample(g_sampler,input.lighttexCoord).r;
	float lightLength = input.lightViewpos.z / input.lightViewpos.w;
	if (texValue + 0.0001 < lightLength)//���C�g�r���[�ł̒������Z���i���C�g�r���[�ł͎Օ���������j
	{
		color /= 3;//�e
	}

	return color;
}