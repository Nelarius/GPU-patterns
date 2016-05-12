#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED

#ifdef DEBUG
#include <cassert>
#define NARWHAL_ASSERT(_EXPR)    assert(_EXPR)
#else
#define NARWHAL_ASSERT(_EXPR)
#endif

#endif // ASSERT_H_INCLUDED
