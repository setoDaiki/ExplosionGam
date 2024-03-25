#include "Random.h"
#include <stdlib.h>
#include <time.h>
#include <cassert>

unsigned int Random::mSeed = -1;

//-----------------------------------------
//    ������
//-----------------------------------------
void Random::Init() {
	mSeed = (unsigned int)time(NULL);   // �����_���̃V�[�h�l�ݒ�
	srand(mSeed);
}

//-------------------------------------------
//   min�`max�̊Ԃ̃����_���Ȑ����l��Ԃ�
//-------------------------------------------
int Random::RandomInt(int min,int max) {
	if (mSeed == -1) {
		Init();
	}
	assert((min <= max) || !("RandomInt: min��max���傫���ł�"));   // min��max�ȉ����U�Ȃ�asset

	return (rand() % (max - min + 1) + min);  // min�`max�͈̔͂���Ԃ�
}


//-------------------------------------------------
//    min�`max�Ń����_���ȕ��������_����Ԃ�
//-------------------------------------------------
float Random::RandomFloat(int min,int max) {
	if (mSeed == -1) {
		Init();
	}
	assert(min <= max || !("RandomFloat: min��max���傫���ł�"));   // min��max�ȉ����U�Ȃ�assret

	int floatRange = 0x7fff;                                              // ������x���m�Ȑ��x�̏����_�ȉ����l�̍ő�l
	float decimalPart = (rand() % (floatRange + 1) / (float)floatRange);  // �������̐��l�ݒ�
	int integerPart = (rand() % (max - min ) + min);                      // �������̐��l�ݒ�
	return decimalPart + static_cast<float>(integerPart);                 // �������ɏ������𑫂��ĕԂ�
}


//----------------------------------------------------------------------------
//   min�`max�Ń����_���ȕ��������_����Ԃ��B���������������_���o�[�W����
//----------------------------------------------------------------------------
float Random::RandomFloat(float min,float max) {
	if (mSeed == -1) {
		Init();
	}
	assert(min <= max || !("RandomFloat: min��max���傫���ł�"));                        // min��max�ȉ����U�Ȃ�assret
	int maxInt = static_cast<int>(max);
	int minInt = static_cast<int>(min);
	int floatRange = 0x7fff;                                                               // ������x���m�Ȑ��x�̏����_�ȉ����l�̍ő�l
	float returnNum = 0.0f;
	do {
		int integerPart = (rand() % (maxInt - minInt) + minInt);                           // �������̐��l�ݒ�
		float decimalPart = (rand() % (floatRange + 1) / static_cast<float>(floatRange));  // �������̐��l�ݒ�
		returnNum = static_cast<float>(integerPart) + decimalPart;
	} while (returnNum <= min || returnNum >= max);                                        // �ő�l�𒴂�����A�ŏ��l�������Ȃ��蒼��
	
	return returnNum;

}
