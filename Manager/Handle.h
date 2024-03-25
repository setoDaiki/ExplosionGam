#pragma once
#include<cassert>


class Handle {
public:
	Handle() 
		:mHandle(0)
	{

	}

	void Init(unsigned index);

	unsigned GetIndex()const {
		return mIndex;
	}
	unsigned GetMagic()const {
		return mMagic;
	}
	unsigned GetHandle()const {
		return mHandle;
	}
	bool IsNull()const {
		return !mHandle;
	}

private:
	unsigned mIndex;
	unsigned mMagic;
	unsigned mHandle;
};



void Handle::Init(unsigned index) {
	assert(IsNull());
	static unsigned sAutoMagic = 0;
	sAutoMagic++;
	mIndex = index;
	mMagic = sAutoMagic;
}
