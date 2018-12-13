#ifndef THSTRING_HPP__
#define THSTRING_HPP__

#include "thstrbase.hpp"
#include <cstring>
#include <cstdio>
#include <cassert>
#include <algorithm>

namespace utd
{

/* TScalar 包含头部信息的字符串或标量值类：
 * 栈对象直接储存短字符串，长字符串用指针指向外部字符串
 * 所指向的字符串，其长度容量信息后偏移获取
 * 不打算支持宽字符，仅限 char 字符，要么推荐 utf8
 * 对象内存布局：8 字节头，外加 8 字节指针
 * 如果不存字符串，指针位置可存一个 8 字节内的数字
 */

#define _TSCALAR TScalar<Alloc>
template <typename Alloc >
class TScalar : THStrBase<0>
{
	// 类型定义
	typedef THStrBase<0> CBase;

public:
	// 常规判断
	bool islocal() const { return _rep.header.head != CBase::LOCAL_MAX; }
	bool isalloc() const { return !islocal() && _rep.header.owned > 0; }
	bool isstring() const { return islocal() || _rep.header.type == 0; }
	bool isnumber() const { return !islocal() && _rep.header.type > 0; }
	bool isstable() const { return !islocal() && _rep.header.stable > 0; }

	// 基础构造设施
	~TScalar() { _free(); }
	TScalar() { _zeroed(); }
	TScalar(const TScalar& that);
	TScalar& operator=(const TScalar& that);

	// 额外参数指定是否信任源，决定是否拷贝
	TScalar(const char* ptr, bool bStable = false);
	TScalar(const TScalar& that, bool bStable);

	// 从一段字符串区间 [pFirst, pLast) 构造
	TScalar(const char* pFirst, const char* pLast);

	// 默认拷贝，但源对象标记为稳定，则不拷贝
	// 稳定性标记在复制时也传递，但修改时始终申请拷贝并标记不稳定
	bool make_stable(bool bStable);

	// 从字符串指针构造一个稳定的 HString 对象
	static TScalar stable(const char* ptr) { return TScalar(ptr, true); }

	// 字符串操作
	char* data() const;
	size_t length() const;
	size_t capacity() const;
	bool empty() const;
	// 总共申请的字符串区块大小
	size_t block() const { return capacity() + _wlen() + _wcap() + 1; }

	// 非 const 操作都可能会触发复制
	const char* c_str() const { return const_cast<const char*>(data()); }
	const char* c_end() const { return c_str() + length(); }
	char* begin() const { return data(); }
	char* end() const { return data() + length(); }

	// 重载 int 下标，支持负索引
	char& operator[](size_t i) { return at(i); }
	const char& operator[](size_t i) const { return of(i); }
	char& operator[](int i) { return at(i); }
	const char& operator[](int i) const { return of(i); }

	char& at(size_t i);
	const char& of(size_t i) const;
	char& at(int i);
	const char& of(int i) const;

	// 比较运算符，字符串长度优先
	bool operator<(const TScalar& that) const { return compare(that) == -1; }
	bool operator==(const TScalar& that) const { return compare(that) == 0; }
	bool operator<(const char* pthat) const { return compare(pthat) == -1; }
	bool operator==(const char* pthat) const { return compare(pthat) == 0; }

	// 返回 -1/0/1
	int compare(const TScalar& that) const ;
	int compare(const char* pthat) const ;

	// 四则运算，字符串只支持加减，数字不能与字符串混合加减
	TScalar& operator+=(const TScalar& that);
	TScalar& operator-=(const TScalar& that);
	TScalar& operator+=(const char* pthat) { return add_by(pthat); }
	TScalar& operator-=(const char* pthat) { return sub_by(pthat); }
	TScalar& operator*=(const TScalar& that) { return times_by(that); }
	TScalar& operator/=(const TScalar& that) { return divid_by(that); }

	TScalar& add_by(const TScalar& that) { return append(that.c_str(), that.length()); }
	TScalar& add_by(const char* pthat) { return append(pthat, strlen(pthat)); }
	TScalar& sub_by(const TScalar& that) { return deppend(that.c_str(), that.length()); }
	TScalar& sub_by(const char* pthat) { return deppend(pthat, strlen(pthat)); }

	TScalar& append(const char* pthat, size_t nLen = 0);
	TScalar& deppend(const char* pthat, size_t nLen = 0);

	// 显式转换，数字操作
	explicit operator const char*() const { return c_str(); }
	explicit operator int() const;
	explicit operator uint32_t() const;
	explicit operator double() const;
	explicit operator int64_t() const;
	explicit operator uint64_t() const;

	TScalar(int iValue);
	TScalar(uint32_t uValue);
	TScalar(double dValue);
	TScalar(int64_t nValue);
	TScalar(uint64_t lValue);

	bool isint() const { return !islocal() && _rep.header.type == CBase::TVALUE_INT; }
	bool isuint() const { return !islocal() && _rep.header.type == CBase::TVALUE_UINT; }
	bool isdouble() const { return !islocal() && _rep.header.type == CBase::TVALUE_DOUBLE; }

	TScalar& add_by_number(const TScalar& that);
	TScalar& sub_by_number(const TScalar& that);
	TScalar& times_by(const TScalar& that);
	TScalar& divid_by(const TScalar& that);

	int compare_by_number(const TScalar& that) const;

	void disp(FILE* fp = stdout) const;
	void disp_number(FILE* fp = stdout) const;
private:
	void _zeroed() { memset(_local, 0, sizeof(*this)); }
	char* _alloc(size_t n);
	void _free();
	void _swap(TScalar& that);

	uint8_t _wlen() const { return _rep.header.backward.wlen; }
	uint8_t _wcap() const { return _rep.header.backward.wcap; }

	// 栈上能储存的最大字符串
	static int _max_local() { return sizeof(TScalar) - 2; }
	// 以三种方式保存字符串
	void _ctor_local(const char* ptr, size_t nLen);
	void _ctor_stable(const char* ptr, size_t nLen);
	void _ctor_alloc(const char* ptr, size_t nLen);

	// 重申请空间扩容
	// nCap 是原容量，This 是原字符串，That 是附加字符串
	void _ctor_recat(size_t nCap, const char* pThis, size_t nThis, const char* pThat, size_t nThat);
	// 申请至少能容纳 nLen 字符串的独立空间
	void _ctor_alloc(size_t nLen);

	// 要修改内容时，检查申请独立空间
	void _clone();

	struct _SRep {
		CBase::SHeader header;
		CBase::UValue value;
	};
	union {
		_SRep _rep;
		char _local[16];
	};
};

template <typename Alloc>
_TSCALAR::TScalar(const char* ptr, bool bStable)
{
	_zeroed();
	size_t nLen = strlen(ptr);
	if (nLen <= _max_local()) {
		_ctor_local(ptr, nLen);
	}
	else if (bStable) {
		_ctor_stable(ptr, nLen);
	}
	else {
		_ctor_alloc(ptr, nLen);
	}
}

template <typename Alloc>
_TSCALAR::TScalar(const TScalar& that)
{
	if (!that.isalloc()) {
		memcpy(&_local[0], &that._local[0], sizeof(*this));
		return;
	}

	_zeroed();
	size_t nLen = that.length();
	const char* ptr = that.c_str();
	if (nLen <= _max_local()) {
		_ctor_local(ptr, nLen);
	}
	else if (that.isstable()) {
		_ctor_stable(ptr, nLen);
	}
	else {
		_ctor_alloc(ptr, nLen);
	}
}

template <typename Alloc>
_TSCALAR::TScalar(const TScalar& that, bool bStable)
{
	if (!that.isalloc()) {
		memcpy(&_local[0], &that._local[0], sizeof(*this));
		return;
	}

	_zeroed();
	size_t nLen = that.length();
	const char* ptr = that.c_str();
	if (nLen <= _max_local()) {
		_ctor_local(ptr, nLen);
	}
	else if (bStable) {
		_ctor_stable(ptr, nLen);
	}
	else {
		_ctor_alloc(ptr, nLen);
	}
}

template <typename Alloc>
_TSCALAR::TScalar(const char* pFirst, const char* pLast)
{
	_zeroed();
	size_t nLen = pLast - pFirst;
	if (nLen <= _max_local()) {
		_ctor_local(pFirst, nLen);
	}
	else {
		_ctor_alloc(pFirst, nLen);
	}
}

template <typename Alloc>
_TSCALAR& _TSCALAR::operator=(const TScalar& that)
{
	if (this != &that) {
		_TSCALAR objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <typename Alloc>
inline
bool _TSCALAR::make_stable(bool bStable)
{
	if (!islocal()) {
		_rep.header.stable = 1;
	}
}

template <typename Alloc>
char* _TSCALAR::data() const
{
	if (islocal()) {
		return const_cast<char*>(&_local[1]);
	}
	if (!isstring()) {
		return NULL;
	}
	return _rep.value.string_;
}

template <typename Alloc>
size_t _TSCALAR::length() const
{
	if (!isstring()) {
		return 0;
	}
	if (islocal()) {
		return _local[0];
	}

	char* ptr = data();
	if (ptr == NULL) {
		return 0;
	}

	if (isstable()) {
		if (_rep.header.length < uint32_t(-1)) {
			return _rep.header.length;
		}
		else {
			return strlen(ptr);
		}
	}

	if (_wlen() > 0) {
		return CBase::DecodeSize(ptr, _wlen());;
	}
	else {
		return strlen(ptr);
	}
}

template <typename Alloc>
size_t _TSCALAR::capacity() const
{
	if (!isstring()) {
		return 0;
	}
	if (islocal()) {
		return _max_local();
	}
	// 未申请自己的空间，容量应为 0
	if (isstable()) {
		return 0;
	}
	return CBase::DecodeSize(data() - _wlen(), _wcap());
}

template <typename Alloc>
inline
bool _TSCALAR::empty() const
{
	if (!isstring()) {
		return uint64_t(*this) == 0;
	}
	char* ptr = data();
	return ptr == NULL || ptr[0] == 0;
}

template <typename Alloc>
inline
char& _TSCALAR::at(size_t i)
{
	if (!islocal() && !isalloc()) { _clone(); }
	return const_cast<char&>(of(i));
}

template <typename Alloc>
inline
char& _TSCALAR::at(int i)
{
	if (!islocal() && !isalloc()) { _clone(); }
	return const_cast<char&>(of(i));
}

template <typename Alloc>
inline
const char& _TSCALAR::of(size_t i) const
{
	return *(data() + i);
}

template <typename Alloc>
const char& _TSCALAR::of(int i) const
{
	if (i >= 0) {
		return of(size_t(i));
	}
	return *(data() + i + length());
}

template <typename Alloc>
int _TSCALAR::compare(const TScalar& that) const
{
	// 数字与数字比较
	if (!isstring() && !that.isstring()) {
		return compare_by_number(that);
	}

	// 字符串与数字，不可比较
	if (isstring() != that.isstring()) {
		return -2;
	}

	// 字符串与字符串比较
	char* pThis = data();
	size_t nThis = length();
	char* pThat = that.data();
	size_t nThat = that.length();

	if (nThis < nThat) { return -1; };
	if (nThat < nThis) { return 1; };
	if (empty()) { return 0; }

	return strcmp(pThis, pThat);
}

template <typename Alloc>
inline
int _TSCALAR::compare(const char* pthat) const
{
	return compare(_TSCALAR(pthat, true));
}

template <typename Alloc>
_TSCALAR& _TSCALAR::operator+=(const TScalar& that)
{
	if (isstring() && that.isstring()) { return add_by(that); }
	if (!isstring() && !that.isstring()) { return add_by_number(that); }
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::operator-=(const TScalar& that)
{
	if (isstring() && that.isstring()) { return sub_by(that); }
	if (!isstring() && !that.isstring()) { return sub_by_number(that); }
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::append(const char* pthat, size_t nNew)
{
	if (!isstring()) { return *this; }

	if (pthat == NULL) { return *this; }
	if (nNew == 0) { nNew = strlen(pthat); }

	size_t nLen = length();
	char* ptr = data();

	// 原字符串在栈上
	if (islocal())
	{
		nLen = _local[0];
		if (nLen + nNew <= _max_local()) {
			strncpy(ptr + nLen, pthat, nNew);
			ptr[nLen+nNew] = '\0';
			_local[0] += nNew;
		}
		else {
			_ctor_recat(nLen, ptr, nLen, pthat, nNew);
		}
		return *this;
	}

	// 原字符串在外部可信指针上
	if (!isalloc()) {
		_ctor_recat(nLen, ptr, nLen, pthat, nNew);
		return *this;
	}

	// 原字符串申请了独立空间
	size_t nCap;
	nCap = CBase::DecodeSize(ptr - _wlen(), _wcap());
	if (nLen + nNew < nCap) {
		strncpy(ptr + nLen, pthat, nNew);
		ptr[nLen+nNew] = '\0';
		CBase::EncodeSize(ptr, _wlen(), nLen + nNew);
	}
	else {
		_ctor_recat(nCap, ptr, nLen, pthat, nNew);
	}
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::deppend(const char* pthat, size_t nThat)
{
	if (!isstring()) { return *this; }

	if (pthat == NULL) { return *this; }
	if (nThat == 0) { nThat = strlen(pthat); }

	char* pThis = data();
	size_t nThis = length();
	if (nThis < nThat) {
		return *this;
	}
	if (strncmp(pThis + nThis - nThat, pthat, nThat) != 0) {
		return *this;
	}

	if (!islocal() && !isalloc()) { 
		_clone(); 
		pThis = data();
	}

	memset(pThis + nThis - nThat, 0, nThat);
	if (islocal()) {
		_local[0] =  nThis - nThat;
	}
	else {
		CBase::EncodeSize(pThis, _wlen(), nThis - nThat);
	}
	return *this;
}

template <typename Alloc>
inline
void _TSCALAR::_ctor_local(const char* ptr, size_t nLen)
{
	// _rep.header.head = CBase::LOCAL_MAX;
	_local[0] = nLen;
	strncpy(&_local[1], ptr, nLen);
	_local[1+nLen] = '\0';
}

template <typename Alloc>
inline
void _TSCALAR::_ctor_stable(const char* ptr, size_t nLen)
{
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = 0;
	_rep.header.owned = 0;
	_rep.header.stable = 1;
	_rep.value.string_ = const_cast<char*>(ptr);
	if (nLen <= uint32_t(-1)) {
		_rep.header.length = nLen;
	}
	else {
		_rep.header.length = uint32_t(-1);
	}
}

template <typename Alloc>
void _TSCALAR::_ctor_alloc(size_t nLen)
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
	_rep.header.type = 0;
	_rep.header.owned = 1;
	_rep.header.stable = 0;
	_rep.header.backward.wlen = iWide;
	_rep.header.backward.wcap = iWide;
	CBase::EncodeSize(pNew + iWide, iWide, nCap);
	// CBase::EncodeSize(pNew + iWide, iWide, nLen);
	_rep.value.string_ = pNew + nHead;
}

template <typename Alloc>
void _TSCALAR::_ctor_alloc(const char* ptr, size_t nLen)
{
	_ctor_alloc(nLen);

	char* dst = data();
	strncpy(dst, ptr, nLen);
	dst[nLen] = '\0';
	CBase::EncodeSize(dst, _wlen(), nLen);
}

template <typename Alloc>
void _TSCALAR::_ctor_recat(size_t nCap, const char* pThis, size_t nThis, const char* pThat, size_t nThat)
{
	// 扩容策略
	if (nCap < nThis) { nCap = nThis; }
	size_t nLen = nThis + nThat;
	size_t nDiff = nLen - nCap;
	size_t nMax = 0;
	if (nCap < 1024) {
		nMax = std::max(2 * nCap, nCap + nDiff);
	}
	else {
		nMax = nCap + nDiff + 1024;
	}

	_TSCALAR objTemp;
	objTemp._ctor_alloc(nMax);

	char* dst = objTemp.data();
	strncpy(dst, pThis, nThis);
	strncpy(dst + nThis, pThat, nThat);
	dst[nLen] = '\0';
	CBase::EncodeSize(dst, objTemp._wlen(), nLen);

	_swap(objTemp);
}

template <typename Alloc>
void _TSCALAR::_clone()
{
	_TSCALAR objTemp;
	objTemp._ctor_alloc(c_str(), length());
	_swap(objTemp);
}

template <typename Alloc>
char* _TSCALAR::_alloc(size_t n)
{
	char* pNew = Alloc().allocate(n);
	memset(pNew, 0, n);
	return pNew;
}

template <typename Alloc>
void _TSCALAR::_free()
{
	if (isalloc()) {
		char* ptr = data();
		if (ptr != NULL) {
			size_t cap = CBase::DecodeSize(ptr - _wlen(), _wcap());
			ptr -= (_wlen() + _wcap());
			cap += 1 + _wlen() + _wcap();
			Alloc().deallocate(ptr, cap);
			_rep.value.string_ = NULL;
		}
	}
}

template <typename Alloc>
void _TSCALAR::_swap(TScalar& that)
{
	char temp[sizeof(*this)];
	memcpy(temp, _local, sizeof(*this));
	memcpy(_local, that._local, sizeof(*this));
	memcpy(that._local, temp, sizeof(*this));
}

// 非字符串操作部分

template <typename Alloc>
_TSCALAR::TScalar(int iValue)
{
	_zeroed();
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = CBase::TVALUE_INT;
	_rep.value.iValue = iValue;
}
template <typename Alloc>
inline
_TSCALAR::operator int() const
{
	return CBase::GetValue<int>(_rep.header.type, _rep.value);
}

template <typename Alloc>
_TSCALAR::TScalar(uint32_t uValue)
{
	_zeroed();
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = CBase::TVALUE_UINT;
	_rep.value.uValue = uValue;
}
template <typename Alloc>
inline
_TSCALAR::operator uint32_t() const
{
	return CBase::GetValue<uint32_t>(_rep.header.type, _rep.value);
}

template <typename Alloc>
_TSCALAR::TScalar(double dValue)
{
	_zeroed();
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = CBase::TVALUE_DOUBLE;
	_rep.value.dValue = dValue;
}
template <typename Alloc>
inline
_TSCALAR::operator double() const
{
	return CBase::GetValue<double>(_rep.header.type, _rep.value);
}

template <typename Alloc>
_TSCALAR::TScalar(int64_t lValue)
{
	_zeroed();
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = CBase::TVALUE_INT;
	_rep.value.iValue = lValue;
}
template <typename Alloc>
inline
_TSCALAR::operator int64_t() const
{
	return CBase::GetValue<int64_t>(_rep.header.type, _rep.value);
}

template <typename Alloc>
_TSCALAR::TScalar(uint64_t uValue)
{
	_zeroed();
	_rep.header.head = CBase::LOCAL_MAX;
	_rep.header.type = CBase::TVALUE_UINT;
	_rep.value.uValue = uValue;
}
template <typename Alloc>
inline
_TSCALAR::operator uint64_t() const
{
	return CBase::GetValue<uint64_t>(_rep.header.type, _rep.value);
}

template <typename Alloc>
_TSCALAR& _TSCALAR::add_by_number(const TScalar& that)
{
	if (isstring() || that.isstring()) { return *this; }
	if (isdouble() || that.isdouble()) {
		_rep.header.type = CBase::TVALUE_DOUBLE;
		_rep.value.dValue = double(*this) + double(that);
	}
	else if (isuint() || that.isuint()) {
		_rep.header.type = CBase::TVALUE_UINT;
		_rep.value.uValue = uint64_t(*this) + uint64_t(that);
	}
	else {
		_rep.header.type = CBase::TVALUE_INT;
		_rep.value.iValue = int64_t(*this) + int64_t(that);
	}
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::sub_by_number(const TScalar& that)
{
	if (isstring() || that.isstring()) { return *this; }
	if (isdouble() || that.isdouble()) {
		_rep.header.type = CBase::TVALUE_DOUBLE;
		_rep.value.dValue = double(*this) - double(that);
	}
	else if (isuint() || that.isuint()) {
		_rep.header.type = CBase::TVALUE_UINT;
		_rep.value.uValue = uint64_t(*this) - uint64_t(that);
	}
	else {
		_rep.header.type = CBase::TVALUE_INT;
		_rep.value.iValue = int64_t(*this) - int64_t(that);
	}
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::times_by(const TScalar& that)
{
	if (isstring() || that.isstring()) { return *this; }
	if (isdouble() || that.isdouble()) {
		_rep.header.type = CBase::TVALUE_DOUBLE;
		_rep.value.dValue = double(*this) * double(that);
	}
	else if (isuint() || that.isuint()) {
		_rep.header.type = CBase::TVALUE_UINT;
		_rep.value.uValue = uint64_t(*this) * uint64_t(that);
	}
	else {
		_rep.header.type = CBase::TVALUE_INT;
		_rep.value.iValue = int64_t(*this) * int64_t(that);
	}
	return *this;
}

template <typename Alloc>
_TSCALAR& _TSCALAR::divid_by(const TScalar& that)
{
	if (isstring() || that.isstring()) { return *this; }
	if (isdouble() || that.isdouble()) {
		_rep.header.type = CBase::TVALUE_DOUBLE;
		_rep.value.dValue = double(*this) / double(that);
	}
	else if (isuint() || that.isuint()) {
		_rep.header.type = CBase::TVALUE_UINT;
		_rep.value.uValue = uint64_t(*this) / uint64_t(that);
	}
	else {
		_rep.header.type = CBase::TVALUE_INT;
		_rep.value.iValue = int64_t(*this) / int64_t(that);
	}
	return *this;
}

template <typename Alloc>
int _TSCALAR::compare_by_number(const TScalar& that) const
{
	if (isdouble() || that.isdouble()) {
		return TFnumberCompare(double(*this), double(that));
	}
	else if (isuint() || that.isuint()) {
		return TFnumberCompare(uint64_t(*this), uint64_t(that));
	}
	else {
		return TFnumberCompare(int64_t(*this), int64_t(that));
	}
	return 0;
}

template <typename Alloc>
void _TSCALAR::disp(FILE* fp) const
{
	// 打印数字
	if (!isstring()) { return disp_number(); }

	// 打印字符串类型
	if (islocal()) {
		fprintf(fp, "%s", "[local]");
	}
	else if (isstable()) {
		fprintf(fp, "%s", "[stable]");
	}
	else {
		fprintf(fp, "%s", "[alloc]");
	}

	fprintf(fp, "[%p][%ld/%ld]->%s", c_str(), length(), capacity(), c_str());
}

template <typename Alloc>
void _TSCALAR::disp_number(FILE* fp) const
{
	switch (_rep.header.type) {
		case TVALUE_STRING:
			fprintf(fp, "%s", "[string?]");
			break;
		case TVALUE_INT:
			fprintf(fp, "%s%ld", "[int]", int64_t(*this));
			break;
		case TVALUE_UINT:
			fprintf(fp, "%s%lu", "[uint]", uint64_t(*this));
			break;
		case TVALUE_DOUBLE:
			fprintf(fp, "%s%g", "[double]", double(*this));
			break;
		default:
			fprintf(fp, "%s", "[unknown]");
	}
}

typedef TScalar<std::allocator<char> >  CScalar;
} /* utd */ 

#endif /* end of include guard: THSTRING_HPP__ */
