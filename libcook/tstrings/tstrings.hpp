#ifndef TSTRINGS_HPP__
#define TSTRINGS_HPP__

#include <cstring>
#include <wchar.h>
#include <stddef.h>

namespace utd
{

// 取字符串长度
template <class CharT>
size_t tstrlen(const CharT* str)
{
	size_t n = 0;
	for (const CharT* p = str; *p != CharT(0); ++p) ++n;
	return n;
}
template<>
inline size_t tstrlen<char>(const char* str) { return strlen(str); }
template<>
inline size_t tstrlen<wchar_t>(const wchar_t* str) { return wcslen(str); }

/* 简单封装字符串指针的类
 * 无独立存储空间，信任指针目标，适用于静态文本或全局容器内的字符串
 * 字符串比较，优先比较长度，因为可利用保存的长度信息
 */
template <class CharT>
class TStr
{
public:
	typedef CharT* pointer;
	typedef const CharT* const_pointer;
	typedef CharT& reference;
	typedef const CharT& const_reference;

	TStr() : length_(0), string_(NULL) {}
	TStr(const CharT* pStr) : length_(tstrlen(pStr)), string_(pStr) {}
	TStr(const CharT* pStr, size_t iLength) : length_(iLength), string_(pStr) {}

	size_t length() const { return length_; }

	// c_str() 与 c_end() 可当作迭代器使用
	const CharT* c_str() const { return string_; }
	const CharT* c_end() const { return c_str() + length(); }
	CharT* begin() { return data(); }
	CharT* end() { return data() + length(); }
	CharT* data() { return const_cast<CharT*>(c_str());}

	// 直接当成C字符串指针使用
	operator const CharT* () const { return c_str();}

	bool operator< (const TStr& that) const
	{
		return length() < that.length() || strcmp(c_str(), that.c_str()) < 0;
	}
	bool operator== (const TStr& that) const
	{
		return c_str() == that.c_str() || (length() == that.length() && strcmp(c_str(), that.c_str()) == 0);
	}
	bool operator> (const TStr& that) const { return that < *this; }
	bool operator<= (const TStr& that) const { return !(that < *this); }
	bool operator>= (const TStr& that) const { return !(*this < that); }
	bool operator!= (const TStr& that) const { return !(*this == that); }

	// 索引未检查越界，像原始字符指针使用，支持负索引
	CharT& operator[] (size_t i) { return *(data() + i); }
	const CharT& operator[] (size_t i) const { return *(c_str() + i); }
	CharT& operator[] (int i)
	{
		if (i < 0) {
			i = length() + i;
		}
		return *(data() + i);
	}
	const CharT& operator[] (int i) const
	{
		if (i < 0) {
			i = length() + i;
		}
		return *(c_str() + i);
	}

protected:
	// 数据布局：存字符串的长度与指针
	size_t   length_;
	const CharT* string_;
};
	
/* 不可变字符串值类，数据布局同父类 TStr
 * 有独立空间字符串，构造时申请复制空间，不可再增长
 * 但每个字符可修改
 */
template <class CharT>
class TString : public TStr<CharT>
{
public:
	// 构造函数系列
	~TString() { _free();}
	TString(const CharT* pStr = NULL) : TStr<CharT>(pStr)
		{ _clone(); }
	TString(const CharT* pStr, size_t iLength) : TStr<CharT>(pStr, iLength)
		{ _clone(); }
	TString(const TString& that) : TStr<CharT>(that.c_str(), that.length())
		{ _clone(); }
	TString(const TStr<CharT>& that) : TStr<CharT>(that.c_str(), that.length())
		{ _clone(); }

	// 赋值操作符
	TString& operator= (const TString& that);
	TString& operator= (const CharT* pthat);

	// 加法重载
	TString operator+ (const CharT* pthat) const;
	TString& operator+= (const CharT* pthat);

protected:
	CharT* _alloc(size_t n);
	void _free();
	void _clone();
	void _swap(TString& that);
}; // end of class TString

template <typename CharT>
TString<CharT>& TString<CharT>::operator= (const TString& that)
{
	if (this != &that) {
		TString objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator= (const CharT* pthat)
{
	TString objTemp(pthat);
	_swap(objTemp);
	return *this;
}

template <typename CharT>
TString<CharT> TString<CharT>::operator+ (const CharT* pthat) const
{
	if (pthat == NULL) {
		return *this;
	}
	TString objTemp(this->string_, this->length_ + tstrlen(pthat));
	if (objTemp.string_ != NULL) {
		strcpy(objTemp.data() + this->length_, pthat);
	}
	return objTemp;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator+= (const CharT* pthat)
{
	if (pthat == NULL) {
		return *this;
	}
	TString objTemp(this->string_, this->length_ + tstrlen(pthat));
	if (objTemp.string_ != NULL) {
		strcpy(objTemp.data() + objTemp.length_, pthat);
	}
	_swap(objTemp);
	return *this;
}

// 申请可存一定字符串的内存空间，自动多申请一个尾部 NULL 字符的空间
template <typename CharT>
CharT* TString<CharT>::_alloc(size_t n)
{
	CharT* pNew = static_cast<CharT*>(::operator new(sizeof(CharT) * (n+1)));
	pNew[n] = CharT(0);
	return pNew;
}

template <typename CharT>
void TString<CharT>::_free()
{
	if (this->string_ != NULL)
	{
		CharT* ptr = this->data();
		::operator delete(ptr);
		this->string_ = NULL;
	}
	this->length_ = 0;
}

// 独立克隆一份字符串，断开原来的字符串指针（不能释放源指针）
template <typename CharT>
void TString<CharT>::_clone()
{
	if (this->length_ == 0 || this->string_ == NULL) {
		return;
	}
	CharT* ptr = _alloc(this->length_);
	if (ptr != NULL) {
		strcpy(ptr, this->string_);
	}
	this->string_ = const_cast<const CharT*>(ptr);
}

// 交换数据，便于实现一些算法，交换指针可利用析构自动释放字符串内存
template <typename CharT>
void TString<CharT>::_swap(TString& that)
{
	size_t iTemp = this->length_; this->length_ = that.length_; that.length_ = iTemp;
	const CharT* pTemp = this->string_; this->string_ = that.string_; that.string_ = pTemp;
}

/* 用于构造可增长的字符串
 * 在父类 TString 的基础上添加预留容量数据
 * 保守扩展内存策略：多申请差额的 2 倍
 */
template <class CharT>
class TStrbuf : public TString<CharT>
{
public:

	// 构造函数系列
	~TStrbuf() { capacity_ = 0; }
	TStrbuf(const CharT* pStr = NULL) : TString<CharT>(pStr)
		{ capacity_ = this->length_; }
	TStrbuf(const CharT* pStr, size_t iLength) : TString<CharT>(pStr, iLength)
		{ capacity_ = this->length_; }
	TStrbuf(const TStrbuf& that) : TString<CharT>(that)
		{ capacity_ = this->length_; }
	TStrbuf(const TStr<CharT>& that) : TString<CharT>(that)
		{ capacity_ = this->length_; }
	TStrbuf(size_t nCap);

	// 赋值操作符
	TStrbuf& operator= (const TStrbuf& that);
	TStrbuf& operator= (const CharT* pthat);

	// 字符串增长，附加在末尾
	TStrbuf& append(const CharT* pthat)
	{
		return _append(pthat, tstrlen(pthat));
	}
	TStrbuf& append(const TStr<CharT>& that)
	{
		return _append(that.c_str(), that.length());
	}
	TStrbuf& operator<< (const CharT* pthat) { return append(pthat); }
	TStrbuf& operator+= (const CharT* pthat) { return append(pthat); }
	TStrbuf& operator<< (const TStr<CharT> that) { return append(that); }
	TStrbuf& operator+= (const TStr<CharT> that) { return append(that); }

	TStrbuf operator+ (const CharT* pthat) const;

	size_t capacity() const { return capacity_; }
	// 扩展空间
	TStrbuf& reserve(size_t nCap);
	// 删除冗余空间，并将自己强转为基类返回，可另外指定截断长度
	TString<CharT>* fixstr(size_t cutlen = 0);

protected:
	void _swap(TStrbuf& that);
	TStrbuf& _append(const CharT* pStr, size_t iLength);
	// 重新申请内存搬迁，有可能更小截断
	void _realloc(size_t n);

private:
	size_t capacity_;
}; // end of class TStrbuf

template <typename CharT>
TStrbuf<CharT>::TStrbuf(size_t nCap)
{
	this->length_ = 0;
	CharT* ptr = this->_alloc(nCap);
	if (ptr != NULL) {
		ptr[0] = CharT(0);
	}
	this->string_ = const_cast<const CharT*>(ptr);
	capacity_ = nCap;
}

template <class CharT>
TStrbuf<CharT>& TStrbuf<CharT>::operator= (const TStrbuf& that)
{
	if (this != &that) {
		TStrbuf objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <class CharT>
TStrbuf<CharT>& TStrbuf<CharT>::operator= (const CharT* pthat)
{
	TStrbuf objTemp(pthat);
	_swap(objTemp);
	return *this;
}

template <class CharT>
TStrbuf<CharT> TStrbuf<CharT>::operator+ (const CharT* pthat) const
{
	if (pthat == NULL) {
		return *this;
	}
	TStrbuf objTemp(this->string_, this->length_ + tstrlen(pthat));
	if (objTemp.string_ != NULL) {
		strcpy(objTemp.data() + this->length_, pthat);
	}
	return objTemp;
}

template <class CharT>
TStrbuf<CharT>& TStrbuf<CharT>::reserve(size_t nCap)
{
	if (nCap > capacity_)
	{
		_realloc(nCap);
	}
	return *this;
}

template <class CharT>
TString<CharT>* TStrbuf<CharT>::fixstr(size_t cutlen/* = 0*/)
{
	size_t nCap = this->length_;
	if (cutlen > 0 && cutlen < this->length_)
	{
		nCap = cutlen;
	}
	_realloc(nCap);
	return this;
}

template <class CharT>
TStrbuf<CharT>& TStrbuf<CharT>::_append(const CharT* pStr, size_t iLength)
{
	size_t newLen = this->length_ + iLength;
	if (newLen > capacity_) {
		size_t nCap = capacity_ + (newLen - capacity_) * 2;
		reserve(nCap);
	}
	strcpy(this->data() + this->length(), pStr);
	this->length_ = newLen;
	return *this;
}

template <class CharT>
void TStrbuf<CharT>::_realloc(size_t n)
{
	CharT* ptr = this->_alloc(n);
	if (ptr == NULL) {
		return;
	}
	if (this->string_ != NULL)
	{
		if (n <= this->length_) {
			strncpy(ptr, this->string_, n);
			this->length_ = n;
		} else {
			strcpy(ptr, this->string_);
		}
		::operator delete(this->data());
	}
	this->string_ = const_cast<const CharT*>(ptr);
	capacity_ = n;
}

template <typename CharT>
void TStrbuf<CharT>::_swap(TStrbuf& that)
{
	TString<CharT>::_swap(that);
	size_t iTemp = capacity_; capacity_ = that.capacity_; that.capacity_ = iTemp;
}

// 定义最常用的 char 类字符串
typedef TStr<char> CStr;
typedef TString<char> CString;
typedef TStrbuf<char> CStrbuf;

} /* utd */ 


#endif /* end of include guard: TSTRINGS_HPP__ */
