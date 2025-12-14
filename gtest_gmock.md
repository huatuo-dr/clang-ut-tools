# 使用Gtest和Gmock

Github仓库：https://github.com/google/googletest

## 📖 介绍

GoogleTest（简称 GTest）是 Google 开发的 C++ 测试框架，GMock 是配套的 Mock 框架。

### ✨ 主要特性

- **丰富的断言宏**：EXPECT_* / ASSERT_* 系列
- **Test Fixtures**：SetUp / TearDown，支持类级别和测试级别
- **参数化测试**：TEST_P + INSTANTIATE_TEST_SUITE_P
- **Death Tests**：测试程序崩溃行为
- **GMock**：强大的 Mock 框架，支持 EXPECT_CALL 设置期望

### ⚠️ 注意事项

- GoogleTest 是 **C++ 框架**，测试文件需要使用 `.cpp` 后缀
- 测试 C 代码时，需要用 `extern "C"` 包裹 C 头文件
- GoogleTest 1.17.0 要求 **C++17** 标准

## 🔧 安装

### 源码使用

省略这种方式，推荐使用下面的方法

### 打包使用

手动下载最新的Gtest Release产物：https://github.com/google/googletest/releases


或者使用如下命令：

```shell
wget https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
tar -xzvf ./googletest-1.17.0.tar.gz
cd ./googletest-1.17.0
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./gtest-install -DCMAKE_BUILD_TYPE=Release -DBUILD_GMOCK=ON ..
make && make install
# 目录gtest-install就是我们需要的产物
```

*`lib`目录中的`cmake`和`pkgconfig`可以删除*

## 📁 项目结构

```text
ut_gtest_gmock/
├── ut.mk                       # GoogleTest编译规则
├── ut_cov.mk                   # 覆盖率编译规则
├── gtest-install/              # GoogleTest库（已编译）
│   ├── include/
│   │   ├── gtest/
│   │   │   └── gtest.h
│   │   └── gmock/
│   │       └── gmock.h
│   └── lib/
│       ├── libgtest.a
│       ├── libgtest_main.a
│       ├── libgmock.a
│       └── libgmock_main.a
└── src/                        # 测试源码
    ├── test_calc.cpp           # calc模块测试（基础断言、参数化测试）
    ├── test_greeting.cpp       # greeting模块测试（字符串断言、Fixtures）
    └── test_multi_calc.cpp     # multi-calc模块测试（GMock）
```

## 🚀 构建命令

```shell
make ut_gtest          # 编译、运行测试并生成报告
make ut_gtest_build    # 仅编译测试（不运行）
make ut_gtest_run      # 运行测试（输出到终端）
make ut_gtest_report   # 生成测试报告

# 覆盖率相关
make ut_gtest_cov      # 运行测试并生成覆盖率报告
make ut_gtest_cov_run  # 仅运行覆盖率测试
make ut_gtest_cov_report # 生成HTML覆盖率报告
make clean-gtest-cov   # 清理覆盖率文件
```

## 📝 GoogleTest 基础教程

### 1️⃣ 最简单的测试用例

```cpp
#include <gtest/gtest.h>

// 测试 C 代码需要 extern "C"
extern "C" {
#include "calc.h"
}

// TEST(测试套件名, 测试名)
TEST(CalcAddTest, PositiveNumbers) {
    EXPECT_EQ(calc_add(2, 3), 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**📌 注意事项：**
- 使用 `TEST(TestSuiteName, TestName)` 定义测试
- `EXPECT_*` 失败后继续执行，`ASSERT_*` 失败后停止当前测试
- `RUN_ALL_TESTS()` 运行所有注册的测试

### 2️⃣ 常用断言宏

#### EXPECT vs ASSERT

| 类型 | 失败后行为 | 使用场景 |
|------|------------|----------|
| `EXPECT_*` | 继续执行 | 非致命检查，收集所有失败 |
| `ASSERT_*` | 停止当前测试 | 致命检查，后续依赖此结果 |

#### 比较断言

```cpp
EXPECT_EQ(val1, val2);     // val1 == val2
EXPECT_NE(val1, val2);     // val1 != val2
EXPECT_LT(val1, val2);     // val1 < val2
EXPECT_LE(val1, val2);     // val1 <= val2
EXPECT_GT(val1, val2);     // val1 > val2
EXPECT_GE(val1, val2);     // val1 >= val2
```

#### 布尔断言

```cpp
EXPECT_TRUE(condition);    // condition 为真
EXPECT_FALSE(condition);   // condition 为假
```

#### 字符串断言

```cpp
EXPECT_STREQ(str1, str2);  // C字符串相等
EXPECT_STRNE(str1, str2);  // C字符串不等
EXPECT_STRCASEEQ(s1, s2);  // 忽略大小写相等
EXPECT_STRCASENE(s1, s2);  // 忽略大小写不等
```

#### 指针断言

```cpp
EXPECT_EQ(ptr, nullptr);   // 指针为空
EXPECT_NE(ptr, nullptr);   // 指针非空
```

### 3️⃣ Test Fixtures（测试夹具）

使用 `TEST_F` 宏配合 Fixture 类：

```cpp
class CalcFixtureTest : public ::testing::Test {
protected:
    int operand_a;
    int operand_b;

    // 每个测试前执行
    void SetUp() override {
        operand_a = 10;
        operand_b = 5;
    }

    // 每个测试后执行
    void TearDown() override {
        // cleanup
    }
};

// 使用 TEST_F 而非 TEST
TEST_F(CalcFixtureTest, AddWithFixture) {
    EXPECT_EQ(calc_add(operand_a, operand_b), 15);
}
```

#### 类级别 Setup/Teardown

```cpp
class MyTest : public ::testing::Test {
protected:
    // 所有测试开始前执行一次
    static void SetUpTestSuite() {
        // 初始化共享资源
    }

    // 所有测试结束后执行一次
    static void TearDownTestSuite() {
        // 清理共享资源
    }
};
```

### 4️⃣ 参数化测试

```cpp
// 测试数据结构
struct CalcAddTestData {
    int a;
    int b;
    int expected;
};

// 参数化测试类
class CalcAddParamTest : public ::testing::TestWithParam<CalcAddTestData> {
};

// 参数化测试用例
TEST_P(CalcAddParamTest, AdditionWorks) {
    const CalcAddTestData& data = GetParam();
    EXPECT_EQ(calc_add(data.a, data.b), data.expected);
}

// 实例化测试数据
INSTANTIATE_TEST_SUITE_P(
    AdditionTests,
    CalcAddParamTest,
    ::testing::Values(
        CalcAddTestData{1, 1, 2},
        CalcAddTestData{0, 0, 0},
        CalcAddTestData{-1, 1, 0},
        CalcAddTestData{100, 200, 300}
    )
);
```

## 📝 GMock 教程

### 1️⃣ Mock C 函数（使用 --wrap）

由于 GMock 主要针对 C++ 类，Mock C 函数需要结合链接器的 `--wrap` 选项：

```cpp
// 声明真实函数（链接器自动提供）
extern "C" {
    extern int __real_calc_add(int a, int b);
}

// Mock 控制
static bool mock_calc_add = true;
static int mock_add_return = 0;

// Wrap 函数
extern "C" {
int __wrap_calc_add(int a, int b) {
    if (mock_calc_add) {
        return mock_add_return;
    }
    return __real_calc_add(a, b);
}
}
```

### 2️⃣ 设置 Mock 返回值

```cpp
TEST_F(MultiCalcMockTest, ExpressionWithMockedValues) {
    // 设置 mock 返回值
    mock_add_return = 5;
    mock_subtract_return = 6;
    mock_multiply_return = 30;

    int result = multi_calc_expression(2, 3, 10, 4);

    EXPECT_EQ(result, 30);
}
```

### 3️⃣ 验证调用

```cpp
TEST_F(MultiCalcMockTest, VerifyCallCounts) {
    mock_add_return = 5;

    multi_calc_expression(1, 2, 3, 4);

    // 验证调用次数
    EXPECT_EQ(add_call_count, 1);
    EXPECT_EQ(subtract_call_count, 1);
    EXPECT_EQ(multiply_call_count, 1);

    // 验证参数
    EXPECT_EQ(last_add_a, 1);
    EXPECT_EQ(last_add_b, 2);
}
```

### 4️⃣ 混合测试（Real + Mock）

```cpp
TEST_F(MultiCalcHybridTest, PartialMock_OnlyMultiply) {
    // 使用真实的 add 和 subtract
    mock_calc_add = false;
    mock_calc_subtract = false;
    // 只 mock multiply
    mock_calc_multiply = true;
    mock_multiply_return = 999;

    int result = multi_calc_expression(2, 3, 10, 4);

    EXPECT_EQ(result, 999);
}
```

## 📊 测试输出示例

运行 `make ut_gtest_run` 后的输出：

```text
========================================
Running GoogleTest + GMock Tests...
========================================

--- Running gtest_test_calc ---
[==========] Running 23 tests from 7 test suites.
[----------] 4 tests from CalcAddTest
[ RUN      ] CalcAddTest.PositiveNumbers
[       OK ] CalcAddTest.PositiveNumbers (0 ms)
...
[==========] 23 tests from 7 test suites ran. (0 ms total)
[  PASSED  ] 23 tests.

--- Running gtest_test_greeting ---
[==========] Running 14 tests from 5 test suites.
...
[  PASSED  ] 14 tests.

--- Running gtest_test_multi_calc (GMock Tests) ---
[==========] Running 10 tests from 2 test suites.
...
[  PASSED  ] 10 tests.
```

## 📊 代码覆盖率

### 使用方法

```bash
# 完整流程：编译、运行测试、生成覆盖率报告
make ut_gtest_cov

# 查看报告
wslview build/coverage-gtest-report/index.html
```

### 报告文件

```text
build/coverage-gtest-report/
├── coverage.info              # lcov 原始数据
├── coverage_filtered.info     # 过滤后的数据
├── index.html                 # HTML 报告入口
└── sdk/src/                   # SDK 源码覆盖详情
```

## 🔄 GoogleTest vs CMocka vs Unity 对比

| 特性 | GoogleTest | CMocka | Unity |
|------|------------|--------|-------|
| **语言** | C++ | C | C |
| **断言风格** | `EXPECT_EQ` | `assert_int_equal` | `TEST_ASSERT_EQUAL_INT` |
| **Fixtures** | SetUp/TearDown (类方法) | setup/teardown (函数指针) | setUp/tearDown (全局函数) |
| **Mock 框架** | GMock (内置) | 内置 will_return/mock | 配合 fff 使用 |
| **参数化测试** | TEST_P + Values | cmocka_unit_test_prestate | 手动循环 |
| **测试注册** | 自动（宏展开） | 手动数组 | RUN_TEST() |
| **输出格式** | --gtest_output=xml | CMOCKA_MESSAGE_OUTPUT | 自定义文本 |

## 📚 本项目展示的特性

| 特性 | 文件 | 说明 |
|------|------|------|
| 基本断言 | test_calc.cpp | EXPECT_EQ, ASSERT_EQ, EXPECT_TRUE |
| 字符串断言 | test_greeting.cpp | EXPECT_STREQ, EXPECT_STRNE |
| Test Fixtures | test_calc.cpp | TEST_F, SetUp, TearDown |
| Suite Fixtures | test_greeting.cpp | SetUpTestSuite, TearDownTestSuite |
| 参数化测试 | test_calc.cpp | TEST_P, INSTANTIATE_TEST_SUITE_P |
| Mock (--wrap) | test_multi_calc.cpp | __wrap_xxx, __real_xxx |
| 混合测试 | test_multi_calc.cpp | 动态切换 Mock/真实函数 |
| XML 报告 | ut.mk | --gtest_output=xml |

## 🔗 参考资料

- [GoogleTest GitHub](https://github.com/google/googletest)
- [GoogleTest Primer](https://google.github.io/googletest/primer.html)
- [GoogleTest Advanced](https://google.github.io/googletest/advanced.html)
- [GMock for Dummies](https://google.github.io/googletest/gmock_for_dummies.html)
