# 使用 GoogleTest 和 MockCpp

官方介绍：https://code.google.com/archive/p/mockcpp/

## 介绍

MockCpp 是一个 C/C++ mock 框架，主要特点：

- **运行时 API Hooking**：无需 `--wrap` 链接选项，直接在运行时 hook 函数
- **不使用 MOCKER() 就调用真实函数**：只有显式 mock 的函数才会被替换
- **链式语法**：`.stubs()` / `.expects()` / `.with()` / `.will()` 风格
- **支持调用次数验证**：`once()`, `exactly(n)`, `atLeast(n)` 等
- **支持参数匹配**：`eq()`, `any()`, `gt()`, `lt()` 等

## 安装

### 源码使用

省略这种方式，推荐使用下面的方法

### 打包使用

手动下载最新的 MockCpp Release 产物：https://code.google.com/archive/p/mockcpp/downloads

*这个产物在编译过程中不支持 python3，还有一个 static_assert 冲突问题*

或者使用如下命令：

```shell
# wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/mockcpp/mockcpp-2.6.tar.gz
# tar -xzvf ./mockcpp-2.6.tar.gz
git clone https://github.com/huatuo-dr/mockcpp
cd mockcpp
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./mockcpp-install ..
make && make install
# 目录 mockcpp-install 就是我们需要的产物
```

## 基本用法

### 头文件引入

```cpp
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>

// C 函数需要 extern "C"
extern "C" {
#include "calc.h"
}

USING_MOCKCPP_NS
```

### Test Fixture 模板

```cpp
class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        GlobalMockObject::reset();  // 重置所有 mock
    }

    void TearDown() override {
        GlobalMockObject::verify(); // 验证期望
        GlobalMockObject::reset();  // 清理 mock
    }
};
```

### Mock 函数返回固定值

```cpp
TEST_F(MyTest, MockReturnFixedValue) {
    MOCKER(calc_add)
        .stubs()
        .will(returnValue(100));

    int result = calc_add(1, 2);  // 返回 100，而不是 3
    EXPECT_EQ(result, 100);
}
```

### 不使用 MOCKER 调用真实函数

```cpp
TEST_F(MyTest, NoMockCallsRealFunction) {
    // 没有 MOCKER()，调用真实实现
    int result = calc_add(2, 3);
    EXPECT_EQ(result, 5);  // 真实结果
}
```

### 参数匹配

```cpp
// 精确匹配参数
MOCKER(calc_add)
    .stubs()
    .with(eq(2), eq(3))      // 只有 calc_add(2, 3) 才返回 999
    .will(returnValue(999));

// 任意参数
MOCKER(calc_multiply)
    .stubs()
    .with(any(), any())      // 任意参数都返回 42
    .will(returnValue(42));
```

### 调用次数验证

```cpp
// 期望只调用一次
MOCKER(calc_add)
    .expects(once())
    .will(returnValue(10));

// 期望调用 N 次
MOCKER(calc_add)
    .expects(exactly(3))
    .will(returnValue(10));

// TearDown 中的 verify() 会检查是否满足期望
```

### 连续调用返回不同值

```cpp
MOCKER(calc_add)
    .stubs()
    .will(returnValue(10))   // 第一次调用返回 10
    .then(returnValue(20))   // 第二次调用返回 20
    .then(returnValue(30));  // 第三次及以后返回 30

EXPECT_EQ(calc_add(0, 0), 10);
EXPECT_EQ(calc_add(0, 0), 20);
EXPECT_EQ(calc_add(0, 0), 30);
EXPECT_EQ(calc_add(0, 0), 30);  // 重复最后一个值
```

## 常用 API 速查

| API | 说明 |
|-----|------|
| `MOCKER(func)` | Mock 指定函数 |
| `.stubs()` | 设置桩（不验证调用次数） |
| `.expects(once())` | 期望调用一次 |
| `.expects(exactly(n))` | 期望调用 n 次 |
| `.expects(atLeast(n))` | 期望至少调用 n 次 |
| `.expects(atMost(n))` | 期望最多调用 n 次 |
| `.with(eq(v))` | 参数等于 v |
| `.with(any())` | 任意参数 |
| `.with(gt(v))` | 参数大于 v |
| `.with(lt(v))` | 参数小于 v |
| `.will(returnValue(v))` | 返回值 v |
| `.then(returnValue(v))` | 下一次调用返回 v |
| `GlobalMockObject::verify()` | 验证所有期望 |
| `GlobalMockObject::reset()` | 重置所有 mock |

## 与其他 Mock 框架对比

| 特性 | MockCpp | CMocka | GMock |
|------|---------|--------|-------|
| Mock 方式 | 运行时 Hook | 链接时 `--wrap` | 虚函数/模板 |
| 需要链接选项 | ❌ 不需要 | ✅ 需要 | ❌ 不需要 |
| 调用真实函数 | 不 MOCKER 即可 | `__real_func()` | 需要 delegate |
| 语言支持 | C/C++ | C | C++ |
| 参数匹配 | ✅ 支持 | ❌ 手动检查 | ✅ 支持 |
| 调用次数验证 | ✅ 支持 | ❌ 手动计数 | ✅ 支持 |

## 项目中的使用

```shell
# 构建并运行测试
make ut_gtest_mockcpp

# 仅构建
make ut_gtest_mockcpp_build

# 仅运行测试
make ut_gtest_mockcpp_run

# 生成测试报告
make ut_gtest_mockcpp_report

# 清理
make clean-ut-gtest-mockcpp
```

测试报告位置：`build/ut-gtest-mockcpp-report/`
