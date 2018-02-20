
// Copyright 2017 Hakan Metin

#ifndef INCLUDE_COSY_LOGGING_H_
#define INCLUDE_COSY_LOGGING_H_

#include <cassert>
#include <iostream>

#include "cosy/IntegralTypes.h"
#include "cosy/Macros.h"

#define DCHECK_GE(value, cond) (assert((value) >= (cond)))
#define DCHECK_LE(value, cond) (assert((value) <= (cond)))
#define DCHECK_GT(value, cond) (assert((value) >  (cond)))
#define DCHECK_LT(value, cond) (assert((value) <  (cond)))
#define DCHECK_NE(value, cond) (assert((value) != (cond)))
#define DCHECK_EQ(value, cond) (assert((value) == (cond)))
#define DCHECK(cond) (assert((cond)))
#define DCHECK_NOTNULL(cond) (assert((cond != nullptr)))

#define CHECK_GE(value, cond) (assert((value) >= (cond)))
#define CHECK_LE(value, cond) (assert((value) <= (cond)))
#define CHECK_GT(value, cond) (assert((value) >  (cond)))
#define CHECK_LT(value, cond) (assert((value) <  (cond)))
#define CHECK_NE(value, cond) (assert((value) !=  (cond)))
#define CHECK_EQ(value, cond) (assert((value) == (cond)))
#define CHECK(cond) (assert((cond)))
#define CHECK_NOTNULL(cond) (assert((cond != nullptr)))


struct X {
    ~X() { std::cout << std::endl; }
};

#define LOG(mode) (X(), std::cout << __FILE__ << ":" << __LINE__ << "] ")

#endif  // INCLUDE_COSY_LOGGING_H_
