#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) (((a) < 0) ? -(a) : (a))
#endif

#ifndef CLAMP
#define CLAMP(x, min, max) MIN(MAX(x, min), max)
#endif

#define CR_EXIT(code) exit(code)

#ifndef CR_INLINE
#ifdef __GNUC__
#define CR_INLINE __inline__
#elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__DMC__) || defined(__SC__) || defined(__WATCOMC__) ||     \
    defined(__LCC__) || defined(__DECC) || defined(__CC_ARM)
#define CR_INLINE __inline
#ifndef __inline__
#define __inline__ __inline
#endif
#else
#define CR_INLINE inline
#ifndef __inline__
#define __inline__ inline
#endif
#endif
#endif /* CR_INLINE not defined */

#ifndef CR_FORCE_INLINE
#ifdef _MSC_VER
#define CR_FORCE_INLINE __forceinline
#elif ((defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__))
#define CR_FORCE_INLINE __attribute__((always_inline)) static __inline__
#else
#define CR_FORCE_INLINE static CR_INLINE
#endif
#endif /* CR_FORCE_INLINE not defined */

#include "Car/Core/Ref.hpp"
#include "Car/Core/Log.hpp"

#define CR_BIND_FN1(fn) std::bind(&fn, this, std::placeholders::_1)

// pre-compiled headers for commonly used headers in the program
#include "Car/Core/crpch.hpp"
