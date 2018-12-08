#ifndef TSTRBUF_HPP__
#define TSTRBUF_HPP__

#include "tstring.hpp"
namespace utd
{

/* 用于构造可增长的字符串
 * 在父类 TString 的基础上添加预留容量数据
 * 保守扩展内存策略：多申请差额的 2 倍
 */
#define _TSTRBUF TStrbuf<CharT, Traits, Alloc>
template <class CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT> >
class TStrbuf : public TString<CharT, Traits, Alloc>
{
public:
	// 构造函数系列
	~TStrbuf() { _free(); }
	TStrbuf() {}
	TStrbuf(const TStrbuf& that) { _copy(that); }
	// 首次从字符串构造，不留冗余空间，将容量同步为长度
	TStrbuf(const CharT* pStr) : _TSTRING(pStr) { _capacity_syn(); }
	TStrbuf(const _TSTR& that) : _TSTRING(that) { _capacity_syn(); }
	TStrbuf(const CharT* pFirst, const CharT* pLast) : _TSTRING(pFirst, pLast) { _capacity_syn(); }
	TStrbuf(size_t nLength, const CharT& chat) : _TSTRING(nLength, chat) { _capacity_syn(); }
	// 预造一个指定容量但零长的字符串
	TStrbuf(size_t nCap);

	// 赋值操作符
	TStrbuf& operator= (const TStrbuf& that);
	TStrbuf& operator= (const _TSTR& that);
	TStrbuf& operator= (const CharT* pthat) { return operator=(_TSTR(pthat)); }

	// 字符串增长，附加在末尾
	TStrbuf& append(const _TSTR& that) { return add_suffix(that); }
	TStrbuf& operator<< (const _TSTR that) { return append(that); }

	// +-/ 运算语义类似 TString ，优化覆盖版
	TStrbuf& operator+= (const _TSTR& that) { return add_suffix(that); }
	TStrbuf& operator-= (const _TSTR& that) { return sub_suffix(that); }
	TStrbuf& operator+= (const CharT& chat) { return add_suffix(chat); }
	TStrbuf& operator-= (const CharT& chat) { return sub_suffix(chat); }
	TStrbuf& operator*= (size_t times) { return repeat(times); }
	TStrbuf& operator/= (size_t times) { return repart(times); }
	// 与数字相加：扩充容量；与数字相加：减少容量
	TStrbuf& operator+= (size_t nCap) { return add_capacity(nCap); }
	TStrbuf& operator-= (size_t nCap) { return sub_capacity(nCap); }

	// 操作符函数别名，在串接字符串时可额外指定一小段（固定）分隔符
	TStrbuf& add_suffix(const _TSTR& that, const CharT* pSep = NULL);
	TStrbuf& sub_suffix(const _TSTR& that, const CharT* pSep = NULL);
	TStrbuf& add_suffix(const CharT& chat, const CharT* pSep = NULL);
	TStrbuf& sub_suffix(const CharT& chat, const CharT* pSep = NULL);
	TStrbuf& repeat(size_t times, const CharT* pSep = NULL);
	TStrbuf& repart(size_t times, const CharT* pSep = NULL);
	TStrbuf& add_capacity(size_t nCap) { return reserve(nCap, +1); }
	TStrbuf& sub_capacity(size_t nCap) { return reserve(nCap, -1); }

	size_t capacity() const { return capacity_; }
	// 扩展空间，额外参数(1/-1)表示相对当前容量，默认传入绝对值
	// 实际扩容会圆整对齐
	TStrbuf& reserve(size_t nCap, int iRelative = 0);
	// 删除冗余空间，并将自己强转为基类返回，可另外指定截断长度
	_TSTRING* shrink(size_t cutlen = 0);

	void disp(FILE* fp = stdout) const;
protected:
	void _capacity_syn() { this->capacity_ = this->length_; }
	void _swap(TStrbuf& that);
	void _copy(const TStrbuf& that);
	void _free();
	// 确保能容纳更长的字符串，参数与 reserve 义同
	// 需要扩容时，会额外申请差量的 2 倍
	void _reserve_length(size_t nLength, int iRelative = 1);
	// 重新申请内存搬迁，有可能更小截断
	void _realloc(size_t n);

	size_t capacity_;
}; // end of class TStrbuf

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF::TStrbuf(size_t nCap)
{
	this->length_ = 0;
	CharT* ptr = this->_alloc(nCap);
	if (ptr != NULL) {
		ptr[0] = CharT(0);
	}
	this->string_ = const_cast<const CharT*>(ptr);
	capacity_ = nCap;
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::_free()
{
	if (this->string_ != NULL)
	{
		CharT* ptr = this->data();
		Alloc().deallocate(ptr, this->capacity_ + 1);
		this->string_ = NULL;
	}
	this->length_ = 0;
	this->capacity_ = 0;
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::_copy(const TStrbuf& that)
{
	this->capacity_ = that.capacity_;
	this->length_ = that.length_;
	if (this->capacity_ <= 0) {
		this->string_ = NULL;
	}
	CharT* ptr = this->_alloc(this->capacity_);
	const CharT* src = that.c_str();
	if (ptr != NULL) {
		if (src == NULL || that.length_ == 0) {
			ptr[0] = CharT(0);
		}
		else {
			Traits::copy(ptr, src, that.length_);
			ptr[that.length_] = CharT(0);
		}
	}
	this->string_ = const_cast<const CharT*>(ptr);
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::operator= (const TStrbuf& that)
{
	if (this != &that) {
		TStrbuf objTemp(that);
		_swap(objTemp);
	}
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::operator= (const _TSTR& that)
{
	TStrbuf objTemp(that);
	_swap(objTemp);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::add_suffix(const _TSTR& that, const CharT* pSep)
{
	if (that.empty()) { return *this; }
	_TSTR strSep(pSep);
	size_t nLarger = strSep.length() + that.length();
	_reserve_length(nLarger);
	CharT* dst = this->data();
	if (dst == NULL) { return *this; }
	dst += this->length_;
	if (pSep != NULL) {
		Traits::copy(dst, strSep.c_str(), strSep.length());
		dst += strSep.length();
	}
	Traits::copy(dst, that.c_str(), that.length());
	dst += that.length();
	*dst = CharT(0);
	this->length_ += nLarger;
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::add_suffix(const CharT& chat, const CharT* pSep)
{
	_TSTR strSep(pSep);
	size_t nLarger = strSep.length() + 1;
	_reserve_length(nLarger);
	CharT* dst = this->data();
	if (dst == NULL) { return *this; }
	dst += this->length_;
	if (pSep != NULL) {
		Traits::copy(dst, strSep.c_str(), strSep.length());
		dst += strSep.length();
	}
	*dst++ = chat;
	*dst = CharT(0);
	this->length_ += nLarger;
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
inline
_TSTRBUF& _TSTRBUF::sub_suffix(const _TSTR& that, const CharT* pSep)
{
	_TSTRING::sub_suffix(that, pSep);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
inline
_TSTRBUF& _TSTRBUF::sub_suffix(const CharT& chat, const CharT* pSep)
{
	_TSTRING::sub_suffix(chat, pSep);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::repeat(size_t times, const CharT* pSep)
{
	if (--times <= 0) { return *this; }
	_TSTR strSep(pSep);
	size_t nLarger = times * (strSep.length() + this->length());
	_reserve_length(nLarger);
	CharT* dst = this->data();
	if (dst == NULL) { return *this; }
	dst += this->length_;
	while (times-- > 0) {
		if (pSep != NULL) {
			Traits::copy(dst, strSep.c_str(), strSep.length());
			dst += strSep.length();
		}
		Traits::copy(dst, this->c_str(), this->length());
		dst += this->length_;
	}
	*dst = CharT(0);
	this->length_ += nLarger;
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::repart(size_t times, const CharT* pSep)
{
	_TSTRING::repart(times, pSep);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRBUF& _TSTRBUF::reserve(size_t nCap, int iRelative)
{
	if (iRelative > 0) {
		nCap = this->capacity_ + nCap;
	}
	else if (iRelative < 0) {
		nCap = this->capacity_ - nCap;
	}

	// 多申请一个 0 字节
	nCap += sizeof(CharT);

	// 保留缓冲，圆整到 8 字节
	static size_t nAlign = 8;
	if (nCap < nAlign) { nCap = nAlign; }
	nCap = (nCap + nAlign - 1) & ~(nAlign - 1);
	_realloc(nCap - 1);
	return *this;
}

template <typename CharT, typename Traits, typename Alloc>
_TSTRING* _TSTRBUF::shrink(size_t cutlen/* = 0*/)
{
	size_t nCap = this->length_;
	if (cutlen > 0 && cutlen < this->length_) {
		nCap = cutlen;
	}
	_realloc(nCap);
	return this;
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::_reserve_length(size_t nLength, int iRelative)
{
	if (iRelative > 0) {
		nLength = this->length_ + nLength;
	}
	else if (iRelative < 0) {
		nLength = this->length_ - nLength;
	}
	if (nLength <= this->capacity_) { return; }
	size_t nLarger = nLength - this->capacity_;
	// 额外申请两倍的差量
	reserve(2*nLarger, 1);
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::_realloc(size_t n)
{
	_TSTRBUF objTemp(n);
	objTemp.length_ = std::min(this->length_, n);
	CharT* ptr = objTemp.data();
	if (ptr == NULL) {
		return;
	}
	Traits::copy(ptr, this->c_str(), objTemp.length_);
	objTemp[objTemp.length_] = CharT(0);
	_swap(objTemp);
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::_swap(TStrbuf& that)
{
	TString<CharT>::_swap(that);
	std::swap(this->capacity_, that.capacity_);
}

template <typename CharT, typename Traits, typename Alloc>
void _TSTRBUF::disp(FILE* fp) const
{
	fprintf(fp, "[0x%x][%d/%d]->%s", reinterpret_cast<size_t>(this->c_str()), this->length(), this->capacity(), this->c_str());
}

typedef TStrbuf<char> CStrbuf;
} /* utd */ 
#endif /* end of include guard: TSTRBUF_HPP__ */
