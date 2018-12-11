#ifndef THSTRBASE_HPP__
#define THSTRBASE_HPP__

#include <cstdint> 
#include <cstddef>

// THString 的基类及相关辅助函数
namespace utd
{
	
// SizeT 是 1/2/4/8 字节的无符号整数
template <class SizeT>
inline 
size_t TFdecodeSize(const char* ptr, SizeT*)
{
	return ptr == NULL ? 0 : *(static_cast<SizeT*>(ptr) - 1);
}

template <class SizeT>
inline 
void TFencodeSize(const char* ptr, SizeT size)
{
	*(static_cast<SizeT*>(ptr) - 1) = size;
}

template <typename NumberT>
int TFnumberCompare(NumberT lhs, NumberT rhs)
{
	if (lhs < rhs) { return -1; }
	if (rhs < lhs) { return +1; }
	return 0;
}

// 伪模板基类
#define _THSTRBASE THStrBase<NVERSION>
template <int NVERSION>
class THStrBase
{
public:
	static int version() { return NVERSION; }

protected:
	// 类型定义

	// 解析的字符串头部信息
	struct SStrHeadValue
	{
		size_t length;
		size_t capacity;
	};

	// 指针字符串后偏移元数据，用多少字节表示长度、容量等信息
	struct SBacker
	{
		uint8_t wlen;
		uint8_t wcap;
		uint8_t wref; // 暂时只用前两字节，后面忽略
		uint8_t wpad;
	};

	// 整个栈对象的头部，8 字节，后面应该接字符串指针
	// owned = 0 表示借用指针，length 暂存外部字符串长度
	// owned = 1 表示自己申请字符串空间（包含额外头部）
	// stable, 承诺自己是稳定字串，被复制时只需拷指针
	struct SHeader
	{
		uint8_t head;
		uint8_t type;
		uint8_t owned;
		uint8_t stable;
		union {
			uint32_t length;
			SBacker backward;
		};
	};

	// 后面 8 字节指针也可以用于保存非字符串数据
	// 直接取最大精度的 64 位整数或双精度
	// type 指示该存什么数据，默认字符串（指针）
	enum EValueType
	{
		TVALUE_STRING = 0,
		TVALUE_INT = 1,
		TVALUE_UINT = 2,
		TVALUE_DOUBLE = 3,
	};

	union UValue
	{
		char* string_;
		int64_t iValue;
		uint64_t uValue;
		double dValue;
		char padding[8]; // 保证 8 字节长
	};

	// 常量定义
	enum EValueConst
	{
		UNIT_SIZE = 8,
		LOCAL_MAX = 255,
	};

	static SStrHeadValue DecodeHead(const char* ptr, const SBacker &stBack);
	static size_t DecodeLength(const char* ptr, const SBacker &stBack);
	static size_t DecodeCapacity(const char* ptr, const SBacker &stBack);
	static size_t DecodeSize(const char* ptr, uint8_t iWide);
	static void EncodeSize(const char* ptr, uint8_t iWide, size_t nSize);
	static size_t MaxSize(uint8_t iWide) { return (1 << (iWide-1)*8) -1; }
	static uint8_t SelectSize(size_t nLength);
	static char& BackHole();

	template <typename NumberT>
	static NumberT GetValue(uint8_t type, UValue value);
};

template <int NVERSION>
inline
_THSTRBASE::SStrHeadValue _THSTRBASE::DecodeHead(const char* ptr, const SBacker &stBack)
{
	_THSTRBASE::SStrHeadValue stValue;
	stValue.length = DecodeSize(ptr, stBack.wlen);
	stValue.capacity = DecodeSize(ptr-stBack.wlen, stBack.wcap);
	return stValue;
}

template <int NVERSION>
inline
size_t _THSTRBASE::DecodeLength(const char* ptr, const SBacker &stBack)
{
	return DecodeSize(ptr, stBack.wlen);
}

template <int NVERSION>
inline
size_t _THSTRBASE::DecodeCapacity(const char* ptr, const SBacker &stBack)
{
	return DecodeSize(ptr-stBack.wlen, stBack.wcap);
}

template <int NVERSION>
size_t _THSTRBASE::DecodeSize(const char* ptr, uint8_t iWide)
{
	if (ptr == NULL || iWide == 0) { return 0; }
	switch (iWide)
	{
	case 1:
		return TFdecodeSize(ptr, static_cast<uint8_t*>(0));
	case 2:
		return TFdecodeSize(ptr, static_cast<uint16_t*>(0));
	case 4:
		return TFdecodeSize(ptr, static_cast<uint32_t*>(0));
	case 8:
		return TFdecodeSize(ptr, static_cast<uint64_t*>(0));
	default:
		// error
		return 0;
	}
}

template <int NVERSION>
void _THSTRBASE::EncodeSize(const char* ptr, uint8_t iWide, size_t nSize)
{
	if (ptr == NULL || iWide == 0) { return; }
	switch (iWide)
	{
		case 1:
			return TFencodeSize(ptr, static_cast<uint8_t>(nSize));
		case 2:
			return TFencodeSize(ptr, static_cast<uint16_t>(nSize));
		case 4:
			return TFencodeSize(ptr, static_cast<uint32_t>(nSize));
		case 8:
			return TFencodeSize(ptr, static_cast<uint64_t>(nSize));
		default:
			// error
			return 0;
	}
}

template <int NVERSION>
uint8_t _THSTRBASE::SelectSize(size_t nLength)
{
	static uint8_t WideList[4] = {1, 2, 4, 8};
	for (int i = 0; i < 4; ++i)
	{
		uint8_t iWide = WideList[i];
		size_t nAll = nLength + iWide * 2; // + 1 
		if (nAll <= MaxSize(iWide))
		{
			return iWide;
		}
	}
}

template <int NVERSION>
char& _THSTRBASE::BackHole()
{
	static char chole;
	return chole;
}

template <int NVERSION>
template <typename NumberT>
NumberT _THSTRBASE::GetValue(uint8_t type, UValue value)
{
	switch (type)
	{
		case TVALUE_STRING = 0:
			return 0;
		case TVALUE_INT = 1:
			return NumberT(value.iValue);
		case TVALUE_UINT = 2:
			return NumberT(value.uValue);
		case TVALUE_DOUBLE = 3:
			return NumberT(value.dValue);
		default:
			return 0;
	}
}

} /* utd */ 

#endif /* end of include guard: THSTRBASE_HPP__ */
