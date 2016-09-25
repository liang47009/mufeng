#ifndef _CEGUIDataContainer_h_
#define _CEGUIDataContainer_h_

#include <stdint.h>

typedef	unsigned long	ulong;

#define NEW_ARRAY_PT(T, count, Allocator) new T[count]
#define DELETE_ARRAY_PT(ptr, T, count, Allocator) delete [] ptr

class RawDataContainer {

public:
	RawDataContainer() :
			mData(0), mSize(0) {
	}

	~RawDataContainer(void) {
		release();
	}
	void setData(uint8_t* data) {
		mData = data;
	}

	uint8_t* getDataPtr(void) {
		return mData;
	}

	const uint8_t* getDataPtr(void) const {
		return mData;
	}

	void setSize(size_t size) {
		mSize = size;
	}

	size_t getSize(void) const {
		return mSize;
	}

	void release(void) {
		if (mData) {
			DELETE_ARRAY_PT(mData, uint8, mSize, RawDataContainer);
			mData = 0;
			mSize = 0;
		}
	}

private:
	uint8_t* mData;
	size_t mSize;
};

#endif	// end of guard _CEGUIDataContainer_h_
