#include "HeapOffset.h"
#include <iostream>
#include <algorithm>

unsigned HeapOffset::mNextResourceHeapOffset = 1;
std::vector<unsigned> HeapOffset::mReleasedOffset;

HeapOffset::HeapOffset()
	:mHeapOffset(-1)
	
{


	if (mReleasedOffset.empty()) {
		// �I�t�Z�b�g�����ԂɎg��
		mHeapOffset = mNextResourceHeapOffset;
		mNextResourceHeapOffset += 1;
	}
	else {
		// ������ꂽ�I�t�Z�b�g�����蓖�Ă�
		mHeapOffset = mReleasedOffset.back();
		mReleasedOffset.pop_back();
	}
}

HeapOffset::~HeapOffset() {
	
}

//-----------------------------------------
//  �g�p���̃q�[�v�I�t�Z�b�g���������
//-----------------------------------------
void HeapOffset::Release() {
	mReleasedOffset.push_back(mHeapOffset);
}


//---------------------------------------------
// ���蓖�Ă�ꂽ�q�[�v�I�t�Z�b�g���擾����
//---------------------------------------------
unsigned HeapOffset::GetHeapOffset() {
	return mHeapOffset;
}

