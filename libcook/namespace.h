#ifndef NAMESPACE_H__
#define NAMESPACE_H__

#ifndef LIB_SPACE
#define LIB_SPACE cook
#endif

#define SPACE_ENTER(ns) namespace ns {
#define SPACE_LEAVE }

#define SPACE_BEGIN SPACE_ENTER(LIB_SPACE)
#define SPACE_END }

#endif /* end of include guard: NAMESPACE_H__ */
