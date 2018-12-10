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
	char* data();
	size_t length();
	size_t capacity();
	bool empty();

	// 取首尾指针可能触发重整到首段空间
	// 非 const 操作都可能会触发复制
	const char* c_str() const { return const_cast<const char*>(data()); }
	const char* c_end() const { return c_str() + length(); }
	char* begin() { return data(); }
	char* end() { return data() + length(); }

	// 单索引不会触发重整空间，但 at 可能会触发复制
	// 支持负索引
	char& at(size_t i);
	const char& of(size_t i) const;
	char& at(int i);
	const char& of(int i) const;

	char& operator[](size_t i) { return at(i); }
	const char& operator[](size_t i) const { return of(i); }
	char& operator[](int i) { return at(i); }
	const char& operator[](int i) const { return of(i); }

	bool operator<(const THSring& that);
	bool operator==(const THSring& that);
	bool operator<(const char* pthat);
	bool operator==(const char* pthat);

	THString& operator+=(const THSring& that);
	THString& operator+=(const char* pthat);

	THString& append(const char* pthat, size_t nLen);

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
	char* _getptr(int p) {	return _rep.value[p].string_; }

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
char* _THSTRING::data()
{
	if (islocal()) {
		return &_local[1];
	}
	_merge();
	return _rep.value[0].string_;
}

template <int NFRAGMENT, typename Alloc>
size_t _THSTRING::length()
{
	if (islocal()) {
		return _local[0];
	}

	if (isstable()) {
		if (_rep.header.length < uint32_t(-1)) {
			return _rep.header.length;
		}
		else {
			return strlen(_getptr(0));
		}
	}

	size_t nLen = 0;
	for (int i = 0; i < NFRAGMENT; ++i) {
		char* ptr = _getptr(i);
		if (ptr != NULL) {
			nLen += CBase::DecodeSize(ptr, _wlen());
		}
	}
	return nLen;
}

template <int NFRAGMENT, typename Alloc>
size_t _THSTRING::capacity()
{
	if (islocal()) {
		return sizeof(_local - 2);
	}

	// 未申请自己的空间，容量应为 0
	if (isstable()) {
		return 0;
	}

	size_t nCap = 0;
	for (int i = 0; i < NFRAGMENT; ++i) {
		char* ptr = _getptr(i);
		if (ptr != NULL) {
			nCap += CBase::DecodeSize(ptr - _wlen(), _wcap());
		}
	}
	return nCap;
}

template <int NFRAGMENT, typename Alloc>
bool _THSTRING::empty()
{
	if (islocal()) {
		return _local[0] == 0 || _local[1] == 0;
	}

	if (isstable()) {
		return _getptr(0) == NULL;
	}

	for (int i = 0; i < NFRAGMENT; ++i) {
		char* ptr = _getptr(i);
		if (ptr != NULL && ptr[0] != '\0') {
			return false;
		}
	}

	return true;
}

template <int NFRAGMENT, typename Alloc>
char& _THSTRING::at(size_t i)
{
	if (!isalloc()) { _clone(); }
	return const_cast<char&>(of(i));
}

template <int NFRAGMENT, typename Alloc>
char& _THSTRING::at(int i)
{
	if (!isalloc()) { _clone(); }
	return const_cast<char&>(of(i));
}

template <int NFRAGMENT, typename Alloc>
const char& _THSTRING::of(size_t i) const
{
	size_t nAll = 0;
	for (int p = 0; p < NFRAGMENT; ++p) {
		char* ptr = _getptr(p);
		if (ptr == NULL)
		{
			break;
		}
		size_t nLen = CBase::DecodeSize(ptr, _wlen());
		if (i < nLen + nAll) {
			return ptr[i - nAll];
		}
		nAll += nLen;
	}

	// error
	return CBase::BackHole();
}

template <int NFRAGMENT, typename Alloc>
const char& _THSTRING::of(int i) const
{
	if (i >= 0) {
		return of(size_t(i));
	}

	// 负索引
	i = -i;
	size_t nAll = 0;
	int p = NFRAGMENT;
	for (p > 0) {
		char* ptr = _getptr(--p);
		if (ptr == NULL) {
			continue;
		}
		size_t nLen = CBase::DecodeSize(ptr, _wlen());
		if (i =< nLen + nAll) {
			return ptr[nLen - (i - nAll)];
		}
		nAll += nLen;
	}

	// error
	return CBase::BackHole();
}

template <int NFRAGMENT, typename Alloc>
bool _THSTRING::operator<(const THSring& that)
{
	if (length() < that.length()) { return true };
	if (that.length() < length()) { return false };
	if (empty()) { return false; }

	char* pThis = _getptr(0);
	int nThis == 0;
	char* pThat = that._getptr(0);
	int nThat = 0;
	while (1)
	{
		if (*pThis == 0)
		{
			if (++nThis < NFRAGMENT && _getptr(nThis) != NULL)
			{
				pThis = _getptr(nThis);
			}
		}
		if (*pThat == 0)
		{
			if (++nThat < NFRAGMENT && that._getptr(nThat) != NULL)
			{
				pThat = that._getptr(nThat);
			}
		}
		if (*pThis < *pThat)
		{
			return true;
		}
		if (*pThat < *pThis)
		{
			return false;
		}
		if (*pThis == 0 || *pThat == 0)
		{
			break;
		}
		++pThis;
		++pThat;
	}

	return false;
}

template <int NFRAGMENT, typename Alloc>
bool _THSTRING::operator==(const THSring& that)
{
	if (that.length() != length()) { return false };
	if (empty()) { return true; }

	char* pThis = _getptr(0);
	int nThis == 0;
	char* pThat = that._getptr(0);
	int nThat = 0;
	while (1)
	{
		if (*pThis == 0)
		{
			if (++nThis < NFRAGMENT && _getptr(nThis) != NULL)
			{
				pThis = _getptr(nThis);
			}
		}
		if (*pThat == 0)
		{
			if (++nThat < NFRAGMENT && that._getptr(nThat) != NULL)
			{
				pThat = that._getptr(nThat);
			}
		}
		if (*pThis != *pThat)
		{
			return false;
		}
		if (*pThis == 0 || *pThat == 0)
		{
			break;
		}
		++pThis;
		++pThat;
	}

	return true;
}

template <int NFRAGMENT, typename Alloc>
inline
bool _THSTRING::operator<(const char* pthat)
{
	return (*this) < _THSTRING(pthat, true);
}

template <int NFRAGMENT, typename Alloc>
inline
bool _THSTRING::operator==(const char* pthat)
{
	return (*this) == _THSTRING(pthat, true);
}

template <int NFRAGMENT, typename Alloc>
THString& _THSTRING::operator+=(const THSring& that)
{
	for (int i = 0; i < NFRAGMENT; ++i)
	{
		char* ptr = _getptr(i);
		if (ptr != NULL && ptr[0] != '\0')
		{
			size_t nLen = CBase::DecodeSize(ptr, _wlen());
			append(ptr, nLen);
		}
	}
	return *this;
}

template <int NFRAGMENT, typename Alloc>
inline
THString& _THSTRING::operator+=(const char* pthat)
{
	return append(pthat, strlen(pthat));
}

template <int NFRAGMENT, typename Alloc>
THString& _THSTRING::append(const char* pthat, size_t nNew)
{
	if (islocal())
	{
		nLen = _local[0];
		if (nLen + nNew <= _max_local())
		{
			char* ptr = &_local[1];
			strncpy(ptr + nLen, pthat, nNew);
			_local[0] += nNew;
			ptr[nLen+nNew] = '\0';
		}
		else
		{
			// 申请 nLen+nNew 空间
		}
		return *this;
	}

	if (isstable())
	{
		size_t nLen = length();
		// 申请 nLen+nNew 空间
	}

	size_t nLen;
	size_t nCap;
	for (int i = NFRAGMENT; i-- > 0;) {
		char* ptr = _getptr(i);
		if (ptr == NULL) { continue; }

		nLen = CBase::DecodeSize(ptr, _wlen());
		nCap = CBase::DecodeSize(ptr - _wlen(), _wcap());
		if (nLen + nNew < nCap) {
			strcpy(ptr, pthat);
		}
		else {
			if (++i < NFRAGMENT)
			{
				// 在下一个指针槽申请空间
			}
			else
			{
				// 在第一个指针槽扩容
			}
		}
	}
}

template <int NFRAGMENT, typename Alloc>
size_t _THSTRING::copyto(char* dst, size_t nMax)
{
	return 0;
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
