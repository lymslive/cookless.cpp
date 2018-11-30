#ifndef TSTRINGS_HPP__
#define TSTRINGS_HPP__

#include "tstr.hpp"

namespace utd
{

// 在末尾增加字符串
template <typename StringT, typename StringU>
const StringT operator+ (const StringT& lhs, const StringU& rhs)
{
	return StringT(lhs) += rhs;
}

template <typename StringT, typename CharT>
const StringT operator+ (const StringT& lhs, const CharT& rhs)
{
	return StringT(lhs) += rhs;
}

// 只在末尾删除特定字符串
template <typename StringT, typename StringU>
const StringT operator- (const StringT& lhs, const StringU& rhs)
{
	return StringT(lhs) -= rhs;
}

template <typename StringT, typename CharT>
const StringT operator- (const StringT& lhs, const CharT& rhs)
{
	return StringT(lhs) -= rhs;
}

// 与数字相乘：将当前字符串重复 x 倍
template <typename StringT>
const StringT operator* (const StringT& lhs, size_t rhs)
{
	return StringT(lhs) *= rhs;
}

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
	TString& operator= (const TStr<CharT>& that);

	// 可修改字符串缓冲区内容
	CharT* data() { return const_cast<CharT*>(this->c_str());}
	CharT* begin() { return data(); }
	CharT* end() { return data() + length(); }
	CharT& operator[] (size_t i) { return *(data() + i); }
	CharT& operator[] (int i);

	// 加法重载
	TString& operator+= (const TStr<CharT>& that);
	TString& operator-= (const TStr<CharT>& that);
	TString& operator+= (const CharT& chat);
	TString& operator-= (const CharT& chat);

	TString& operator*= (size_t times);

protected:
	CharT* _alloc(size_t n);
	void _free();
	void _clone();
	void _swap(TString& that);
}; // end of class TString

template <typename CharT>
CharT& TString<CharT>::operator[] (int i)
{
	if (i < 0) {
		i = length() + i;
	}
	return *(data() + i);
}

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
TString<CharT>& TString<CharT>::operator= (const TStr<CharT>& that)
{
	TString objTemp(that);
	_swap(objTemp);
	return *this;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator+= (const TStr<CharT>& that)
{
	if (that.empty()) {
		return *this;
	}
	TString objTemp(this->string_, this->length_ + that.length());
	if (objTemp.string_ != NULL) {
		strcpy(objTemp.data() + objTemp.length_, pthat);
	}
	_swap(objTemp);
	return *this;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator+= (const CharT& chat)
{
	TString objTemp(this->string_, this->length_ + 1);
	if (objTemp.string_ != NULL) {
		objTemp[-1] = chat;
	}
	_swap(objTemp);
	return *this;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator-= (const TStr<CharT>& that)
{
	if (that.empty() || this->emtpy()) {
		return *this;
	}
	// 查看后缀的起始位置，直接抹 0 
	size_t iPos = this->suffix_of(that);
	if (iPos == -1 || iPos >= this->length_) {
		return *this;
	}
	(*this)[iPos] = CharT(0);
	this->length_ = iPos;
	return *this;
}

template <typename CharT>
TString<CharT>& TString<CharT>::operator-= (const CharT& chat)
{
	if (this->emtpy()) {
		return *this;
	}
	if ((*this)[-1] != chat)
	{
		return *this;
	}
	// 直接在源地址上填 0 抹除
	(*this)[--this->length_] = CharT(0);
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
// 可以预设 this->length_ 比源 string_ 指针长
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

typedef TString<char> CString;
} /* utd */ 


#endif /* end of include guard: TSTRINGS_HPP__ */
