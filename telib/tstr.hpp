#ifndef TSTR_HPP__
#define TSTR_HPP__

#include <string> // for char_traits
namespace utd
{

// using std::char_traits;

/* 简单封装字符串指针的类
 * 无独立存储空间，信任指针目标，适用于静态文本或全局容器内的字符串
 * 字符串比较，优先比较长度，因为可利用保存的长度信息
 */
#define _TSTR TStr<CharT, Traits>
template <typename CharT, typename Traits = std::char_traits<CharT> >
class TStr
{
public:
	typedef CharT* pointer;
	typedef const CharT* const_pointer;
	typedef CharT& reference;
	typedef const CharT& const_reference;

	// 拷贝与赋值构造函数按默认即可。主要实现从 C 字符串构造
	TStr(const CharT* pStr = NULL) : length_(0), string_(NULL)
	{
		if (pStr != NULL) {
			length_ = Traits::length(pStr);
			string_ = pStr;
		}
	}

	size_t length() const { return length_; }
	bool empty() const { return length_ == 0 || string_ == NULL; }

	// c_str() 与 c_end() 可当作迭代器使用
	const CharT* c_str() const { return string_; }
	const CharT* c_end() const { return c_str() + length(); }

	// 索引未检查越界，像原始字符指针使用，支持负索引
	const CharT& operator[] (size_t i) const { return *(c_str() + i); }
	const CharT& operator[] (int i) const;

	// 直接当成C字符串指针使用
	operator const CharT* () const { return c_str();}

	// 比较运算符
	bool operator< (const TStr& that) const
	{
		return length() < that.length() || Traits::compare(c_str(), that.c_str(), that.length()) < 0;
	}
	bool operator== (const TStr& that) const
	{
		return c_str() == that.c_str() || (length() == that.length() && Traits::compare(c_str(), that.c_str(), length()) == 0);
	}
	bool operator> (const TStr& that) const { return that < *this; }
	bool operator<= (const TStr& that) const { return !(that < *this); }
	bool operator>= (const TStr& that) const { return !(*this < that); }
	bool operator!= (const TStr& that) const { return !(*this == that); }

	// 判断前后缀
	bool has_prefix(const TStr& that) const;
	bool has_suffix(const TStr& that) const { return suffix_of(that) != -1; }
	// 查找后缀的起始索引，不存在返回 -1
	size_t suffix_of(const TStr& that) const;

protected:
	// 数据布局：存字符串的长度与指针
	size_t   length_;
	const CharT* string_;
};

template <typename CharT, typename Traits> inline
const CharT& _TSTR::operator[] (int i) const
{
	if (i < 0) {
		i = length() + i;
	}
	return *(c_str() + i);
}

template <typename CharT, typename Traits>
bool _TSTR::has_prefix(const _TSTR& that) const
{
	if (that.empty()) {
		return true;
	}

	if (this->length() < that.length()) {
		return false;
	}

	return Traits::compare(this->c_str(), that.c_str(), that.length()) == 0;
}

template <typename CharT, typename Traits>
size_t _TSTR::suffix_of(const _TSTR& that) const
{
	if (that.empty()) {
		return true;
	}

	if (this->length() < that.length()) {
		return false;
	}

	const CharT *pThat = that.c_str(); 
	const CharT *pThis = this->c_str() + this->length() - that.length();
	return Traits::compare(pThis, pThat, that.length()) == 0;
}

typedef TStr<char> CStr;
} /* utd */ 
#endif /* end of include guard: TSTR_HPP__ */
