

namespace Circular {


template<typename IndexType >
struct Index {

	Index( const IndexType minIndexValue,
		   const IndexType maxIndexValue,
		   const IndexType initalIndexValue ) :
			   _indexMin( minIndexValue ),
			   _indexMax( maxIndexValue ),
			   _index( initalIndexValue ),
			   _totalSize(maxIndexValue - minIndexValue) {};

	Index& operator+=( const IndexType size ) {
		// Calculate value have to add
		const IndexType appendValue = ( size > _totalSize ) ? ( size % _totalSize ) : size;


#if 0
		// Get value.
		const IndexType newValue = ( _index + appendValue );
		// So, mistake is possible, (appendValue + _index) > MaxValueOf(IndexType)
		if ( newValue > _indexMax ) {
			const IndexType newAroundValueWithoutOffset = newValue % _indexMax;
			const IndexType newAroundValue = newAroundValueWithoutOffset + _indexMin;
			_index = newAroundValue;
		} else {
			_index = newValue;
		}
#endif
		return *this;
	}

	Index& operator-=( const IndexType size ) {
		// Calculate value have to substract
		const IndexType substractValue = (size > _totalSize) ? ( size % _totalSize ) : size;
		// Get value.


		return *this;
	}

	inline const IndexType value() const {
		return _index;
	}

private:
	const IndexType	_indexMin;
	const IndexType	_indexMax;
	IndexType		_index;
	const IndexType _totalSize;
};


enum class CirculaBufferState {
	Free,
	Allocated,
	Commited,
	Used
};

template<typename IndexType>
struct CircularBufferDescriptor {
	CircularIndex<IndexType> 	_startPos{};
	IndexType					_dataOffset;
	IndexType					_totalSize;
	CirculaBufferState			_state;
};


}

