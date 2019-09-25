#ifndef H5712E60C_DEF5_4A3B_93C1_C074CD80C63C
#define H5712E60C_DEF5_4A3B_93C1_C074CD80C63C

#include "cub/base/status.h"
#include "cub/log/log.h"

#define __CUB_FAILED_BOOL_ACTION return false;
#define __CUB_FAILED_STATUS_ACTION return cub::Failure;
#define __CUB_STATUS_ACTION return __status;
#define __CUB_VOID_ACTION return;
#define __CUB_NIL_ACTION return 0;
#define __CUB_PEEK_ACTION
#define __CUB_RESULT_ACTION(status) return status;

//////////////////////////////////////////////////////////////////////////
#define __CUB_WARNING_NOT_TRUE(exp) WARN_LOG << "assertion failed: " #exp

#define __CUB_EXPECT_TRUE(exp, action) \
  do {                                 \
    if (!(exp)) {                      \
      __CUB_WARNING_NOT_TRUE(exp);     \
      action;                          \
    }                                  \
  } while (0)

#define CUB_EXPECT_TRUE_R(exp, status) __CUB_EXPECT_TRUE(exp, __CUB_RESULT_ACTION(status))

#define CUB_EXPECT_TRUE(exp) __CUB_EXPECT_TRUE(exp, __CUB_FAILED_STATUS_ACTION)

#define CUB_EXPECT_FALSE(exp) __CUB_EXPECT_TRUE(!(exp), __CUB_FAILED_STATUS_ACTION)

#define CUB_EXPECT_FALSE_R(exp, status) __CUB_EXPECT_TRUE(!(exp), __CUB_RESULT_ACTION(status))

#define CUB_EXPECT_TRUE_NIL(exp) __CUB_EXPECT_TRUE(exp, __CUB_NIL_ACTION)

#define CUB_EXPECT_TRUE_VOID(exp) __CUB_EXPECT_TRUE(exp, __CUB_VOID_ACTION)

#define CUB_EXPECT_TRUE_BOOL(exp) __CUB_EXPECT_TRUE(exp, __CUB_FAILED_BOOL_ACTION)

///////////////////////////////////////////////////////////////
#define __CUB_NOT_TRUE(exp) ERR_LOG << "assertion failed: " #exp

#define __CUB_ASSERT_TRUE(exp, action) \
  do {                                 \
    if (!(exp)) {                      \
      __CUB_NOT_TRUE(exp);             \
      action;                          \
    }                                  \
  } while (0)

#define CUB_ASSERT_TRUE_R(exp, status) __CUB_ASSERT_TRUE(exp, __CUB_RESULT_ACTION(status))

#define CUB_ASSERT_TRUE(exp) __CUB_ASSERT_TRUE(exp, __CUB_FAILED_STATUS_ACTION)

#define CUB_ASSERT_FALSE(exp) __CUB_ASSERT_TRUE(!(exp), __CUB_FAILED_STATUS_ACTION)

#define CUB_ASSERT_FALSE_R(exp, status) __CUB_ASSERT_TRUE(!(exp), __CUB_RESULT_ACTION(status))

#define CUB_ASSERT_TRUE_NIL(exp) __CUB_ASSERT_TRUE(exp, __CUB_NIL_ACTION)

#define CUB_PEEK_TRUE(exp) __CUB_ASSERT_TRUE(exp, __CUB_PEEK_ACTION)

#define CUB_ASSERT_TRUE_VOID(exp) __CUB_ASSERT_TRUE(exp, __CUB_VOID_ACTION)

#define CUB_ASSERT_TRUE_BOOL(exp) __CUB_ASSERT_TRUE(exp, __CUB_FAILED_BOOL_ACTION)

//////////////////////////////////////////////////////////////////////////
#define __CUB_ASSERT_TRUE_POST(exp, action, returnAction) \
  do {                                                    \
    if (!(exp)) {                                         \
      __CUB_NOT_TRUE(exp);                                \
      action;                                             \
      returnAction;                                       \
    }                                                     \
  } while (0)

#define CUB_ASSERT_TRUE_POST(exp, action) __CUB_ASSERT_TRUE_POST(exp, action, __CUB_FAILED_STATUS_ACTION)

//////////////////////////////////////////////////////////////////////////
#define __CUB_WARNING_CALL(call, status) WARN_LOG << #call " got " << status

#define __CUB_EXPECT_SUCC_CALL(call, action) \
  do {                                       \
    auto __status = call;                    \
    if (__CUB_FAILED(__status)) {            \
      __CUB_WARNING_CALL(call, __status);    \
      action;                                \
    }                                        \
  } while (0)

#define CUB_EXPECT_SUCC_CALL_R(call, status) __CUB_EXPECT_SUCC_CALL(call, __CUB_RESULT_ACTION(status))

#define CUB_EXPECT_SUCC_CALL(call) __CUB_EXPECT_SUCC_CALL(call, __CUB_STATUS_ACTION)

#define CUB_EXPECT_SUCC_CALL_BOOL(call) __CUB_EXPECT_SUCC_CALL(call, __CUB_FAILED_BOOL_ACTION)

#define CUB_EXPECT_SUCC_CALL_VOID(call) __CUB_EXPECT_SUCC_CALL(call, __CUB_VOID_ACTION)

#define CUB_EXPECT_SUCC_CALL_NIL(call) __CUB_EXPECT_SUCC_CALL(call, __CUB_NIL_ACTION)

/////////////////////////////////////////////////////////////////////
#define __CUB_FAIL_CALL(call, status) ERR_LOG << #call " got " << status

#define __CUB_ASSERT_SUCC_CALL(call, action) \
  do {                                       \
    cub::Status __status = call;             \
    if (__CUB_FAILED(__status)) {            \
      __CUB_FAIL_CALL(call, __status);       \
      action;                                \
    }                                        \
  } while (0)

#define CUB_ASSERT_SUCC_CALL_R(call, status) __CUB_ASSERT_SUCC_CALL(call, __CUB_RESULT_ACTION(status))

#define CUB_ASSERT_SUCC_CALL(call) __CUB_ASSERT_SUCC_CALL(call, __CUB_STATUS_ACTION)

#define CUB_ASSERT_SUCC_CALL_BOOL(call) __CUB_ASSERT_SUCC_CALL(call, __CUB_FAILED_BOOL_ACTION)

#define CUB_ASSERT_SUCC_CALL_VOID(call) __CUB_ASSERT_SUCC_CALL(call, __CUB_VOID_ACTION)

#define CUB_ASSERT_SUCC_CALL_NIL(call) __CUB_ASSERT_SUCC_CALL(call, __CUB_NIL_ACTION)

/*no return even though call failed.*/
#define CUB_PEEK_SUCC_CALL(call) __CUB_ASSERT_SUCC_CALL(call, __CUB_PEEK_ACTION)

//////////////////////////////////////////////////////////////////////////

#define __CUB_ASSERT_SUCC_CALL_FINALLY(call, action, returnAction) \
  do {                                                             \
    cub::Status __status = call;                                   \
    if (__CUB_FAILED(__status)) {                                  \
      __CUB_FAIL_CALL(call, __status);                             \
      action;                                                      \
      returnAction;                                                \
    }                                                              \
  } while (0)

#define CUB_ASSERT_SUCC_CALL_FINALLY(exp, action) \
  __CUB_ASSERT_SUCC_CALL_FINALLY(exp, action, __CUB_FAILED_STATUS_ACTION)

//////////////////////////////////////////////////////////////////////////
#define __CUB_ASSERT_NO_REAL_FAIL(call, action) \
  do {                                          \
    cub::Status __status = call;                \
    if (__status == CUB_NOTHING_CHANGED) {      \
      return CUB_SUCCESS;                       \
    } else if (__CUB_FAILED(__status)) {        \
      __CUB_FAIL_CALL(call, __status);          \
      action;                                   \
    }                                           \
  } while (0)

#define CUB_ASSERT_NO_REAL_FAIL(call) __CUB_ASSERT_NO_REAL_FAIL(call, __CUB_STATUS_ACTION)

//////////////////////////////////////////////////////////////////////////
#define __CUB_NIL_PTR(ptr) ERR_LOG << "assertion failed: unexpected null ptr: " #ptr

#define __CUB_ASSERT_VALID_PTR(ptr, action) \
  do {                                      \
    if ((ptr) == nullptr) {                 \
      __CUB_NIL_PTR(ptr);                   \
      action;                               \
    }                                       \
  } while (0)

#define CUB_ASSERT_VALID_PTR_R(ptr, status) __CUB_ASSERT_VALID_PTR(ptr, __CUB_RESULT_ACTION(status))

#define CUB_ASSERT_VALID_PTR(ptr) __CUB_ASSERT_VALID_PTR(ptr, __CUB_FAILED_STATUS_ACTION)

#define CUB_ASSERT_VALID_PTR_BOOL(ptr) __CUB_ASSERT_VALID_PTR(ptr, __CUB_FAILED_BOOL_ACTION)

#define CUB_ASSERT_VALID_PTR_VOID(ptr) __CUB_ASSERT_VALID_PTR(ptr, __CUB_VOID_ACTION)

#define CUB_PEEK_VALID_PTR(ptr) __CUB_ASSERT_VALID_PTR(ptr, __CUB_PEEK_ACTION)

#define CUB_ASSERT_VALID_PTR_NIL(ptr) __CUB_ASSERT_VALID_PTR(ptr, __CUB_NIL_ACTION)

#define __CUB_ASSERT_VALID_PTR_FINALLY(ptr, action, returnAction) \
  if ((ptr) == nullptr) {                                         \
    __CUB_NIL_PTR(ptr);                                           \
    action;                                                       \
    returnAction;                                                 \
  }

#define CUB_PEEK_VALID_PTR_FINALLY(ptr, action) __CUB_ASSERT_VALID_PTR_FINALLY(ptr, action, __CUB_PEEK_ACTION)

#define CUB_ASSERT_VALID_PTR_NIL_FINALLY(ptr, action) __CUB_ASSERT_VALID_PTR_FINALLY(ptr, action, __CUB_NIL_ACTION)

#endif
