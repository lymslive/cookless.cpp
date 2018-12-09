#ifndef THSTRING_HPP__
#define THSTRING_HPP__

#include <cstring>
#include <cassert>

namespace utd
{

/* THString 包含头部信息的字符串类：
 * 栈对象直接储存短字符串，长字符串用指针指向外部字符串
 * 所指向的字符串，其长度容量信息后偏移获取
 * 不打算支持宽字符，仅限 char 字符，要么推荐 utf8
 */

/* 对象内存布局：
 * 1. 直接定长字符数组
 * 2. 8 字节头加剩余 8 字节指针（或值）数组
 * NFRAGMENT < 256/8 -1
 */ 
#define _THSTRING THString<NFRAGMENT, Alloc>
template <int NFRAGMENT, typename Alloc = std::allocator<char> >
class THString : THStrBase<0>
{
	// 类型定义
	typedef THStringBase<0> CBase;

public:
	// 常规判断
	bool islocal() const { return _rep.header.head != CBase::LOCAL_MAX; }
	bool isalloc() const { return !islocal() && _rep.header.owned > 0; }
	bool nostring() const { return !islocal() && _rep.header.type > 0; }
	bool isstable() const { return !islocal() && _rep.header.stable > 0; }

	// 基础构造设施
	~THString() { _free(); }
	THString() { _zeroed(); }
	THString(const THString& that);
	THString& operator=(const THString& that);

	// 额外参数指定是否信任源，决定是否拷贝
	THString(const char* pStr, bool bStable = false);
	THString(const THString& that, bool bStable);

	// 默认拷贝，但源对象标记为稳定，则不拷贝
	// 稳定性标记在复制时也传递，但修改时始终申请拷贝并标记不稳定
	bool mark_stable(bool bStable);

	// 从字符串指针构造一个稳定的 HString 对象
	static THString stable(const char* pStr) { return THString(pStr, true); }

	// 字符串操作
	const char* c_str();
	const char* c_end();
	char* begin();
	char* end();

	char* data();
	size_t length();
	size_t capacity();

	const char& operator[](size_t i) const;
	char& operator[](size_t i);

	bool operator<(const THSring& that);
	bool operator==(const THSring& that);
	bool operator<(const char* pthat);
	bool operator==(const char* pthat);

	THString& operator+=(const THSring& that);
	THString& operator+=(const char* pthat);

	// 将字符串拷出去
	size_t copyto(char* dst, size_t nMax);

	// 显式转换
	explicit operator const char*() const { return c_str(); }
	explicit operator int() const;
	explicit operator uint32_t() const;
	explicit operator double() const;
	explicit operator int64_t() const;
	explicit operator size_t() const;
	explicit operator const void*() const;

private:
	void _zeroed();
	char* _alloc(size_t n);
	void _free();
	void _swap(const THString& that);

	uint8_t& _wlen() { return _rep.header.backward.wlen; }
	uint8_t& _wcap() { return _rep.header.backward.wcap; }

	// 栈上能储存的最大字符串
	static int _max_local() { return (NFRAGMENT + 1) * 8 - 2; }
	void _ctor_local(const char* pStr, size_t nLen);
	void _ctor_stable(const char* pStr, size_t nLen);
	void _ctor_alloc(const char* pStr, size_t nLen);

	// 要修改内容时，检查申请独立空间
	void _on_modify();

	// 将多段字符串合并至首段，向外界提供连续字符串空间
	// 调用 c_str() data() 前需判断合并
	void _merge();

	struct _SRep {
		CBase::SHeader header;
		CBase::UValue value[NFRAGMENT];
	};
	union {
		_SRep _rep;
		char* _local[8 * (1+NFRAGMENT)];
	};
};

template <int NFRAGMENT, typename Alloc>
_THSTRING::THString(const char* pStr, bool bStable)
{
	_zeroed();
	size_t nLen = strlen(pStr);
	if (nLen <= _max_local()) {
		_ctor_local(pStr, nLen);
	}
	else if (bStable) {
		_ctor_stable(pstr, nLen);
	}
	else {
		_ctor_alloc(pstr, nLen);
	}
}

template <int NFRAGMENT, typename Alloc>
_THSTRING::THString(const THString& that)
{
	_zeroed();
	size_t nLen = that.length();
	const char* pStr = that.c_str();
	if (nLen <= _max_local()) {
		_ctor_local(pStr, nLen);
	}
	else if (that.isstable()) {
		_ctor_stable(pStr, nLen);
	}
	else {
		_ctor_alloc(pStr, nLen);
	}
}

template <int NFRAGMENT, typename Alloc>
_THSTRING::THString(const THString& that, bool bStable)
{
	_zeroed();
	size_t nLen = that.length();
	const char* pStr = that.c_str();
	if (nLen <= _max_local()) {
		_ctor_local(pStr, nLen);
	}
	else if (bStable) {
		_ctor_stable(pStr, nLen);
	}
	else {
		_ctor_alloc(pStr, nLen);
	}
}

template <int NFRAGMENT, typename Alloc>
_THSTRING& _THSTRING::operator=(const THString& that)
{
	if (this != &that) {
		_THSTRING objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <int NFRAGMENT, typename Alloc>
inline
bool _THSTRING::mark_stable(bool bStable)
{
	if (!islocal()) {
		_rep.header.stable = 1;
	}
}

template <int NFRAGMENT, typename Alloc>
inline
void _THSTRING::_ctor_local(const char* pStr, size_t nLen)
{
	_rep.header.head = nLen;
	strcpy(&_local[1], pStr);
}

template <int NFRAGMENT, typename Alloc>
inline
void _THSTRING::_ctor_stable(const char* pStr, size_t nLen)
{
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.owned = 0;
	_rep.header.length = nLen;
	_rep.value[0].string_ = pStr;
}

template <int NFRAGMENT, typename Alloc>
void _THSTRING::_ctor_alloc(const char* pStr, size_t nLen)
{
	uint8_t iWide = CBase::SelectSize(nLen);
	size_t nHead = iWide + iWide;
	size_t nAll = nHead + nLen + 1;

	// 圆整至 8 字节
	static size_t nAlign = 8;
	nAll = (nAll + nAlign - 1) & ~(nAlign - 1);
	if (nAll - 1 > CBase::MaxSize(iWide)) {
		assert(nAll - 1 <= CBase::MaxSize(iWide));
	}
	size_t nCap = nAll - nHead - 1;

	char* pNew = _alloc(nAll);
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.owned = 1;
	_rep.header.backward.wlen = iWide;
	_rep.header.backward.wcap = iWide;
	CBase::EncodeSize(pNew, iWide, nCap);
	CBase::EncodeSize(pNew + iWide, iWide, nLen);
	_rep.value[0].string_ = pNew + nHead;
}

template <int NFRAGMENT, typename Alloc>
inline
void _THSTRING::_zeroed()
{
	memset(_local, 0, sizeof(_local));
}

template <int NFRAGMENT, typename Alloc>
void _THSTRING::_alloc(size_t n)
{
	char* pNew = Alloc().allocate(n);
	pNew[n-1] = char(0);
	return pNew;
}

template <int NFRAGMENT, typename Alloc>
void _THSTRING::_free()
{
	if (!isalloc()) { return; }
	for (int i = 0; i < NFRAGMENT; ++i)
	{
		char* ptr = _rep.value[0].string_;
		if (ptr != NULL)
		{
			size_t cap = CBase::DecodeCapacity(ptr, _rep.header.backward);
			ptr -= (_wlen() + _wcap());
			cap += 1 + _wlen() + _wcap();
			Alloc().deallocate(ptr, cap);
		}
	}
}

template <int NFRAGMENT, typename Alloc>
void _THSTRING::_swap(const THString& that)
{
	char temp[sizeof(_local)];
	memcpy(temp, _local, sizeof(_local));
	memcpy(_local, that._local, sizeof(_local));
	memcpy(that._local, temp, sizeof(_local));
}

typedef THSring<1> str_16t;
typedef THSring<3> str_32t;

} /* utd */ 

#endif /* end of include guard: THSTRING_HPP__ */
