#ifndef TSTRBUF_HPP__
#define TSTRBUF_HPP__

#include "tstring.hpp"
namespace utd
{

// 与数字相加：扩充容量
template <typename StringT>
const StringT operator+ (const StringT& lhs, size_t rhs)
{
	return StringT(lhs) += rhs;
}

// 与数字相加：减少容量
template <typename StringT>
const StringT operator- (const StringT& lhs, size_t rhs)
{
	return StringT(lhs) -= rhs;
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
	TStrbuf& operator= (const TStr<CharT>& that);

	// 字符串增长，附加在末尾
	TStrbuf& append(const TStr<CharT>& that)
	{
		return _append(that.c_str(), that.length());
	}
	TStrbuf& operator<< (const TStr<CharT> that) { return append(that); }
	TStrbuf& operator+= (const TStr<CharT> that) { return append(that); }
	TStrbuf& operator-= (const TStr<CharT>& that);
	TStrbuf& operator+= (const CharT& chat);
	TStrbuf& operator-= (const CharT& chat);
	TStrbuf& operator*= (size_t times);
	TStrbuf& operator+= (size_t nCap);
	TStrbuf& operator-= (size_t nCap);

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
TStrbuf<CharT>& TStrbuf<CharT>::operator= (const TStr<CharT>& that)
{
	TStrbuf objTemp(that);
	_swap(objTemp);
	return *this;
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

typedef TStrbuf<char> CStrbuf;
} /* utd */ 
#endif /* end of include guard: TSTRBUF_HPP__ */
