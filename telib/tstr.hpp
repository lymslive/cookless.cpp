#ifndef TSTR_HPP__
#define TSTR_HPP__

namespace utd
{

// 取字符串长度 
template <typename CharT>
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
template <typename CharT>
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

template <typename CharT>
const CharT& TStr<CharT>::operator[] (int i) const
{
	if (i < 0) {
		i = length() + i;
	}
	return *(c_str() + i);
}

typedef TStr<char> CStr;
} /* utd */ 
#endif /* end of include guard: TSTR_HPP__ */
