#pragma once
#include <vector>
#include <cassert>

template <typename DATA,typename HANDLE>
class HandleManager {
public:
	HandleManager() {};
	~HandleManager() {};

	DATA* Acquire(HANDLE& handle);
	void Release(HANDLE handle);

	DATA* Dereference(HANDLE handle);
	const DATA* Dereference(HANDLE handle)const;

	unsigned GetUsedHandleCount()const {
		return ( mMagicNumbers.size() - mFreeSlots.size() );
	}




private:
	std::vector<DATA> mUserDatas;
	std::vector<unsigned> mMagicNumbers;
	std::vector<unsigned> mFreeSlots;

};



template <typename DATA,typename HANDLE>
DATA* HandleManager<DATA, HANDLE>::Acquire(HANDLE& handle) {
	unsigned index;
	if (mFreeSlots.empty()) {
		index = mMagicNumbers.size();
		handle.Init(index);
		mUserDatas.push_back(DATA());
		mMagicNumbers.push_back(handle.GetMagic());
	}
	else {
		index = mFreeSlots.back();
		handle.Init(index);
		mFreeSlots.pop_back();
		mMagicNumbers[index] = handle.GetMagic();
	}
	return (mUserDatas.begin() + index);
}


template<typename DATA,typename HANDLE>
void HandleManager<DATA, HANDLE>::Release(HANDLE handle) {
	unsigned index = handle.GetIndex();
	assert(index < mUserDatas.size());
	assert(mMagicNumbers[index] == handle.GetMagic());

	mMagicNumbers[index] = 0;
	mFreeSlots.push_back(index);
}


template<typename DATA, typename HANDLE>
DATA* HandleManager<DATA, HANDLE>::Dereference(HANDLE handle) {
	if (handle.IsNull()) return nullptr;

	unsigned index = handle.GetIndex();
	if ( (index >= mUserDatas.size()) || (mMagicNumbers[index] != handle.GetMagic())) {
		assert(!"–³Œø‚Èƒnƒ“ƒhƒ‹");
	}

	return (mUserDatas.begin() + index);

}


template <typename DATA, typename HANDLE>
const DATA* HandleManager<DATA, HANDLE>::Dereference(HANDLE handle) const{
	using ThisType = HandleManager<DATA, HANDLE>;
	return (const_cast<ThisType*>(this)->Dereference(handle));
}
