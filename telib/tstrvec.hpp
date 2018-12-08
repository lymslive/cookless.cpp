#ifndef TSTRVEC_HPP__
#define TSTRVEC_HPP__

namespace utd
{
	
/* 利用 TStrbuf 打造成一个字符串数组（私有继承）
 * 线性存储空间，每个子段用零字符分隔，连续零字符则表示数组结束
 * 对象的缓冲区可视为非空字符串数组，只不适合允许中间有空子串
 */

#define _TSTRVEC TStrvec<CharT, Traits, Alloc>
template <class CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT> >
class TStrvec : private TStrbuf<CharT, Traits, Alloc>
{
public:
	// 基础构造函数
	~TStrvec() {}
	TStrvec() {}
	TStrvec(const TStrvec& that);
	TStrvec& operator= (const TStrvec& that);

	// 继承构造方式
	TStrvec(const CharT* pStr) : _TSTRBUF(pStr), count(1) {}
	TStrvec(const _TSTR& that) : _TSTRBUF(that), count(1) {}
	TStrvec(const CharT* pFirst, const CharT* pLast) : _TSTRBUF(pFirst, pLast), count(1) {}
	TStrvec(size_t nLength, const Char& chat) : _TSTRBUF(nLength, chat), count(1) {}
	TStrvec(size_t nCap) : _TSTRBUF(nCap), count(1) {}

	// 构造 nCount 个字符串，每串长 nLength，全初始化为 chat
	TStrvec(size_t nLength, const Char& chat, size_t nCount);

	// 从一个字符串指针数据构造
	TStrvec(const CharT** pStrs, size_t nCount);
	// 从 TStr 对象的标准向量构造
	TStrvec(std::vector<_TSTR>& vecStr);

	// 获取底层数据
	_TSTRBUF& str_buffer() { return static_cast<_TSTRBUF>&(*this); }
	size_t str_count() { return count_; }

	const CharT* str_head() { return this->string_; }
	bool has_vector() { return count_ > 1; }

	// 转化为 TStr 标准向量，由参数传入指针
	size_t str_vector(std::vector<_TSTR>* pVecStr);

	// 索引子串，允许负索引
	_TSTR operator[] (size_t idx) const;
	_TSTR operator[] (int i) const;

	// 添加/删除一个子串
	TStrvec& operator+= (const _TSTR& that) { return str_add(that); }
	TStrvec& operator-= (const _TSTR& that) { return str_remove(that); }
	TStrvec& operator-= (size_t index) { return str_remove(index); }

	TStrvec& str_add(const _TSTR& that);
	TStrvec& str_remove(const _TSTR& that);
	TStrvec& str_remove(size_t index);

	// 从初始单一字符串分隔子串，填零结束每个子段，压缩连续零
	// 会修改对象本身，c_str() 将只能取到第一子段
	// 返值表示分隔成几段( > 2)
	// 默认分隔方式：空白字符，包括换行符
	size_t split();
	size_t split(CharT chat);
	size_t split(const CharT* pSep);
	// 传入字符串指针与长度时，按另一种方式分隔
	// 字符串中任一字符都可当作分隔符，有 n 种方式
	size_t split(const CharT* pSep, size_t n);
	// 按回车换符符分隔
	size_t splitln();

	// 子串数据迭代器（只读）
	template <class CharT, typename Traits = std::char_traits<CharT>, typename Alloc = std::allocator<CharT> >
	class CStrIterator
	{
	public:
		_TSTR& operator* () { return current_; }
		_TSTR* operator-> () { return &current_; }
		CStrIterator& operator++ ();
		CStrIterator  operator++ (int);
	private:
		_TSTR current_; // 当前子串
		_TSTRVEC* hostbuf_; // 宿主对象
		size_t index_; // 在宿主子串索引序号
	};
	typedef CStrIterator<CharT, Traits, Alloc> str_iterator;

	// split 分隔成数组后取迭代器才有意义
	str_iterator begin();
	str_iterator end();

	// 重新合并为一个长字符串，当作基类返回
	_TSTRBUF& join(const CharT& chat = CharT(10));
	_TSTRBUF& join(const CharT* pSep);

	// 丢弃自 index 之后的子串，缓冲区重置清零
	// 默认只保留第一串
	_TSTRVEC& str_drop(size_t index = 1);

	void disp(FILE* fp = stdout) const;
private:
	size_t count_;
};

typedef TStrvec<char> CStrvec;
} /* utd */ 

#endif /* end of include guard: TSTRVEC_HPP__ */
