#ifndef TSTRINGS_HPP__
#define TSTRINGS_HPP__

#include "tstr.hpp"
#include <memory> // for allocator
#include <algorithm> // for swap

namespace utd
{

/* 不可变字符串值类，数据布局同父类 TStr
 * 有独立空间字符串，构造时申请复制空间，不可再增长
 * 但每个字符可修改
 */
#define _TSTRING TString<CharT, Traits, Alloc>
template <typename CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT> >
class TString : public TStr<CharT, Traits>
{
public:
	// 构造函数系列
	~TString() { _free();}
	TString() {}
	TString(const TString& that);
	// 从常规字符串构造，信任字符串以 NULL 字符结尾
	TString(const CharT* pStr) : _TSTR(pStr) { _clone(); }
	TString(const _TSTR& that) : _TSTR(that) { _clone(); }
	// 拷贝一段字符串区间 [First, Last) ，Last 未必以 NULL 字符结尾
	TString(const CharT* pFirst, const CharT* pLast);
	// 构造一个定长字符串，填充以特定字符
	TString(size_t nLength, const CharT& chat);

	// 赋值操作符
	TString& operator= (const TString& that);
	TString& operator= (const _TSTR& that);
	TString& operator= (const CharT* pthat) { return operator=(_TSTR(pthat)); }

	// 可修改字符串缓冲区内容
	CharT* data() { return const_cast<CharT*>(this->c_str());}
	CharT* begin() { return data(); }
	CharT* end() { return data() + this->length(); }
	CharT& operator[] (size_t i) { return *(data() + i); }
	CharT& operator[] (int i); // 允许负索引

	/* 操作符重载
	// +) 在末尾增加字符串
	// -) 只在末尾删除特定字符串，加法逆运算
	// *) 与数字相乘，将当前字符串重复 x 倍
	// /) 与数字相除，乘法逆运算，只保留开始部分
	*/
	TString& operator+= (const _TSTR& that) { return add_suffix(that); }
	TString& operator-= (const _TSTR& that) { return sub_suffix(that); }
	TString& operator+= (const CharT& chat) { return add_suffix(chat); }
	TString& operator-= (const CharT& chat) { return sub_suffix(chat); }
	TString& operator*= (size_t times) { return repeat(times); }
	TString& operator/= (size_t times) { return repart(times); }

	// 操作符函数别名，在串接字符串时可额外指定一小段（固定）分隔符
	TString& add_suffix(const _TSTR& that, const CharT* pSep = NULL);
	TString& sub_suffix(const _TSTR& that, const CharT* pSep = NULL);
	TString& add_suffix(const CharT& chat, const CharT* pSep = NULL);
	TString& sub_suffix(const CharT& chat, const CharT* pSep = NULL);
	TString& repeat(size_t times, const CharT* pSep = NULL);
	TString& repart(size_t times, const CharT* pSep = NULL);

	// 逆序转换
	TString& reverse();

protected:
	CharT* _alloc(size_t n);
	void _free();
	void _clone(size_t nLength = 0);
	void _swap(TString& that);

private:
	// 在源字符串基础上扩展预期长度，为扩充字符串内部使用
	TString(const _TSTR& that, size_t enlarge);
}; // end of class TString

template <typename CharT, typename Traits, typename Alloc>
CharT& _TSTRING::operator[] (int i)
{
	if (i < 0) {
		i = this->length() + i;
	}
	return *(data() + i);
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING::TString(const TString& that)
{
	this->length_ = that.length();
	this->string_ = that.c_str();
	_clone();
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING::TString(const _TSTR& that, size_t enlarge)
{
	this->length_ = that.length() + enlarge;
	this->string_ = that.c_str();
	_clone(that.length());
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING::TString(const CharT* pFirst, const CharT* pLast)
{
	this->length_ = pLast - pFirst;
	this->string_ = pFirst;
	_clone();
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING::TString(size_t nLength, const CharT& chat)
{
	this->length_ = nLength;
	CharT* ptr = _alloc(this->length_);
	if (ptr != NULL) {
		while (nLength-- > 0) ptr[nLength] = chat;
	}
	this->string_ = const_cast<const CharT*>(ptr);
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::operator= (const TString& that)
{
	if (this != &that) {
		TString objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::operator= (const _TSTR& that)
{
	TString objTemp(that);
	_swap(objTemp);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::add_suffix(const _TSTR& that, const CharT* pSep)
{
	if (that.empty()) {
		return *this;
	}

	_TSTR strSep(pSep);
	size_t nLarger = strSep.length() + that.length();
	TString objTemp(*this, nLarger);
	if (objTemp.string_ != NULL) {
		if (pSep != NULL) {
			Traits::copy(objTemp.data() + this->length_, strSep.c_str(), strSep.length());
		}
		Traits::copy(objTemp.data() + this->length_ + strSep.length(), that.c_str(), that.length());
	}
	_swap(objTemp);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::add_suffix(const CharT& chat, const CharT* pSep)
{
	_TSTR strSep(pSep);
	TString objTemp(*this, strSep.length() + 1);
	if (objTemp.string_ != NULL) {
		if (pSep != NULL) {
			Traits::copy(objTemp.data() + this->length_, strSep.c_str(), strSep.length());
		}
		objTemp[-1] = chat;
	}
	_swap(objTemp);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::sub_suffix(const _TSTR& that, const CharT* pSep)
{
	if (that.empty() || this->empty()) {
		return *this;
	}
	if (!this->has_suffix(that)) {
		return *this;
	}
	// 直接后缀的起始位置抹 0 
	this->length_ -= that.length();
	(*this)[this->length_] = CharT(0);
	// 再抹除可能的后缀分隔符
	if (pSep != NULL) {
		sub_suffix(_TSTR(pSep));
	}
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::sub_suffix(const CharT& chat, const CharT* pSep)
{
	if (this->empty()) {
		return *this;
	}
	if ((*this)[-1] != chat)
	{
		return *this;
	}
	// 直接在源地址上填 0 抹除
	(*this)[--this->length_] = CharT(0);
	if (pSep != NULL) {
		sub_suffix(_TSTR(pSep));
	}
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::repeat(size_t times, const CharT* pSep)
{
	if (--times <= 0) {
		return *this;
	}
	_TSTR strSep(pSep);
	size_t nLarger = times * (strSep.length() + this->length());
	TString objTemp(*this, nLarger);
	if (objTemp.string_ == NULL) {
		return *this;
	}
	CharT *dst = objTemp.data() + this->length();
	while (times-- > 0) {
		if (pSep != NULL) {
			Traits::copy(dst, strSep.c_str(), strSep.length());
			dst += strSep.length();
		}
		Traits::copy(dst, this->c_str(), this->length());
		dst += this->length();
	}
	_swap(objTemp);
	return *this;
}

/* 切分字符串，两种工作方式
 * pSep==NULL 无分隔符，按数字 times 整除向下取整截断，取第一部分
 * pSep!=NULL 有分隔符，按分隔符切分串，取前 times 部分
 *   取第 1 部分 times = 1，如果为 0 不切分
 */
template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::repart(size_t times, const CharT* pSep)
{
	if (pSep == NULL) {
		if (times <= 1) {
			return *this;
		}
		this->length_ /= times;
		(*this)[this->length_] = CharT(0);
	}
	else {
		size_t iPos = _TSTR::findsub(pSep, times);
		if (iPos != -1)
		{
			(*this)[iPos] = CharT(0);
			this->length_ = iPos;
		}
	}
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING& _TSTRING::reverse()
{
	CharT* pFirst = this->begin();
	CharT* pLast = this->end();
	while ((pFirst != pLast) && (pFirst != --pLast)) {
		std::swap(*pFirst++, *pLast);
	}
	return *this;
}

// 申请可存一定字符串的内存空间，自动多申请一个尾部 NULL 字符的空间
template <typename CharT, typename Traits, typename Alloc>
CharT* _TSTRING::_alloc(size_t n)
{
	CharT* pNew = Alloc().allocate(n+1);
	pNew[n] = CharT(0);
	return pNew;
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRING::_free()
{
	if (this->string_ != NULL)
	{
		CharT* ptr = this->data();
		Alloc().deallocate(ptr, this->length() + 1);
		this->string_ = NULL;
	}
	this->length_ = 0;
}

// 独立克隆一份字符串，断开原来的字符串指针（不能释放源指针）
// 可以预设 this->length_ 比源 string_ 指针长，或拷贝部分字符串
template <typename CharT, typename Traits, typename Alloc>
void _TSTRING::_clone(size_t nLength)
{
	if (this->length_ == 0 || this->string_ == NULL) {
		return;
	}
	CharT* ptr = _alloc(this->length_);
	if (nLength <= 0) {
		nLength = this->length_;
	}
	if (ptr != NULL) {
		Traits::copy(ptr, this->string_, nLength);
		if (nLength < this->length_) {
			(*this)[nLength] = CharT(0);
		}
	}
	this->string_ = const_cast<const CharT*>(ptr);
}

// 交换数据，便于实现一些算法，交换指针可利用析构自动释放字符串内存
template <typename CharT, typename Traits, typename Alloc>
inline
void _TSTRING::_swap(TString& that)
{
	std::swap(this->length_, that.length_);
	std::swap(this->string_, that.string_);
}

typedef TString<char> CString;
} /* utd */ 


#endif /* end of include guard: TSTRINGS_HPP__ */
