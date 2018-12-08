#ifndef TOPERATOR_HPP__
#define TOPERATOR_HPP__

// 标准重载运算符模板
// 相关运算符重载，只重载基础的，其他按习惯语义调用基础重载
namespace utd
{

// 比较函数以 == < 为基础
// 用于 > 需反转操作符时，右参数类型要能转化为左参数类型，保证调用同类成员函数
template <typename ThisT, typename ThatT>
bool operator> (const ThisT& lhs, const ThatT& rhs) { return ThisT(rhs) < lhs; }
template <typename ThisT, typename ThatT>
bool operator<= (const ThisT& lhs, const ThatT& rhs) { return !(ThisT(rhs) < lhs); }
template <typename ThisT, typename ThatT>
bool operator>= (const ThisT& lhs, const ThatT& rhs) { return !(lhs < rhs); }
template <typename ThisT, typename ThatT>
bool operator!= (const ThisT& lhs, const ThatT& rhs) { return !(lhs == rhs); }

// +-*/ 用相应的 += 代替
// 效率不是最高，但语义一致
template <typename ThisT, typename ThatT>
const ThisT operator+ (const ThisT& lhs, const ThatT& rhs) { return ThisT(lhs) += rhs; }
template <typename ThisT, typename ThatT>
const ThisT operator- (const ThisT& lhs, const ThatT& rhs) { return ThisT(lhs) -= rhs; }
template <typename ThisT, typename ThatT>
const ThisT operator* (const ThisT& lhs, const ThatT& rhs) { return ThisT(lhs) *= rhs; }
template <typename ThisT, typename ThatT>
const ThisT operator/ (const ThisT& lhs, const ThatT& rhs) { return ThisT(lhs) /= rhs; }

}

#endif /* end of include guard: TOPERATOR_HPP__ */
