#pragma once
#include <vector>
#include <cassert>
#include <Windows.h>

//------------------------------------------------------------------------------------
/// 使用上の注意
/// ヒープオフセット解放にはRelease()を呼び出す。この呼び出しを忘れないようにしてもらいたい
/// 理由. コピーが存在する場合、同じオフセットを二回解放されてしまうから
//-----------------------------------------------------------------------------------

class HeapOffset {
public:
	HeapOffset();
	~HeapOffset();

	HeapOffset(const HeapOffset&) = default;

	void Release();

	unsigned GetHeapOffset();


private:
	unsigned mHeapOffset;                      // 使用しているヒープオフセット
	static unsigned mNextResourceHeapOffset;   // 次のリソースのヒープオフセット
	static std::vector<unsigned> mReleasedOffset;  // 解放されたヒープオフセット
	
};



