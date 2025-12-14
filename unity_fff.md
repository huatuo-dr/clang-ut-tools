# Unity框架配合fff

Unity仓库：https://github.com/ThrowTheSwitch/Unity.git

fff仓库：https://github.com/meekrosoft/fff

## 📖 介绍

### 1. Unity 是什么？

Unity 是一个轻量级 C 语言单元测试框架，主要作用是：

- 提供断言（判断测试是否通过）
- 管理测试用例
- 输出测试结果

### 2. fff 是什么？

fff（Fake Function Framework）是一个函数桩（Mock / Fake）框架，主要解决：

> 被测函数依赖了外部函数，但你在单元测试中：
>
> - 不想真的调用它
> - 或它还没实现
> - 或它依赖硬件 / 系统资源

fff 可以做到：

- 伪造（fake）函数实现
- 记录函数是否被调用
- 记录调用次数、参数
- 设置返回值序列
- 自定义fake函数实现

## 🔧 安装

### 源码使用

省略这种方式，推荐使用下面的方法

### 打包使用

手动下载最新的Unity Release产物：https://github.com/ThrowTheSwitch/Unity/releases

手动下载最新的fff Release产物：https://github.com/meekrosoft/fff/releases

或者使用如下命令：

```shell
# Unity
wget https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.6.1.tar.gz
tar -xzvf ./v2.6.1.tar.gz
cd Unity-2.6.1
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=./unity-install ..
make && make install
# 目录unity-install就是我们需要的产物

cd ../../

# fff
wget https://github.com/meekrosoft/fff/archive/refs/tags/v1.1.tar.gz
tar -xzvf ./v1.1.tar.gz
cd fff-1.1
# fff是纯头文件框架，只需要fff.h文件
```

## 📁 项目结构

```text
ut_unity_fff/
├── ut.mk                       # Unity测试编译规则
├── unity-install/              # Unity库（已编译）
│   ├── include/unity/
│   │   ├── unity.h
│   │   └── unity_internals.h
│   └── lib/
│       └── libunity.a
├── fff-install/                # fff头文件
│   └── fff.h
└── src/                        # 测试源码
    ├── test_calc.c             # calc模块测试（基础断言）
    ├── test_greeting.c         # greeting模块测试（fixtures）
    └── test_multi_calc.c       # multi-calc模块测试（fff mock）
```

## 🚀 构建命令

```shell
make ut_unity          # 编译、运行测试并生成报告
make ut_unity_build    # 仅编译测试（不运行）
make ut_unity_run      # 运行测试（输出到终端）
make ut_unity_report   # 生成测试报告

# 覆盖率相关
make ut_unity_cov      # 运行测试并生成覆盖率报告
make ut_unity_cov_run  # 仅运行覆盖率测试
make ut_unity_cov_report # 生成HTML覆盖率报告
make clean-unity-cov   # 清理覆盖率文件
```

## 📝 Unity基础教程

### 1️⃣ 最简单的测试用例

```c
#include "unity.h"

/* Unity要求必须定义这两个函数 */
void setUp(void) {
    /* 每个测试前执行 */
}

void tearDown(void) {
    /* 每个测试后执行 */
}

/* 测试函数 */
static void test_example(void) {
    TEST_ASSERT_TRUE(1 == 1);
}

int main(void) {
    UNITY_BEGIN();          /* 开始测试 */
    RUN_TEST(test_example); /* 运行测试 */
    return UNITY_END();     /* 结束并返回结果 */
}
```

**📌 注意事项：**
- `setUp()` 和 `tearDown()` 必须定义（即使为空）
- 使用 `RUN_TEST()` 注册测试函数
- `UNITY_BEGIN()` 和 `UNITY_END()` 包裹测试

### 2️⃣ 常用断言宏

#### 整数断言

```c
TEST_ASSERT_EQUAL_INT(expected, actual);        /* 相等 */
TEST_ASSERT_NOT_EQUAL(expected, actual);        /* 不等 */
TEST_ASSERT_GREATER_THAN(threshold, actual);    /* 大于 */
TEST_ASSERT_LESS_THAN(threshold, actual);       /* 小于 */
TEST_ASSERT_INT_WITHIN(delta, expected, actual); /* 范围内 */
```

#### 布尔断言

```c
TEST_ASSERT_TRUE(condition);   /* 为真 */
TEST_ASSERT_FALSE(condition);  /* 为假 */
```

#### 指针断言

```c
TEST_ASSERT_NULL(ptr);         /* 为NULL */
TEST_ASSERT_NOT_NULL(ptr);     /* 非NULL */
TEST_ASSERT_EQUAL_PTR(expected, actual);  /* 指针相等 */
```

#### 字符串断言

```c
TEST_ASSERT_EQUAL_STRING(expected, actual);         /* 字符串相等 */
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len); /* 前n个字符相等 */
```

#### 内存断言

```c
TEST_ASSERT_EQUAL_MEMORY(expected, actual, len);  /* 内存块相等 */
```

### 3️⃣ Fixtures（测试夹具）

Unity的setUp/tearDown在**每个测试**前后执行：

```c
static char *buffer = NULL;

void setUp(void) {
    buffer = (char *)malloc(256);
}

void tearDown(void) {
    free(buffer);
    buffer = NULL;
}

static void test_use_buffer(void) {
    TEST_ASSERT_NOT_NULL(buffer);
    strcpy(buffer, "Hello");
    TEST_ASSERT_EQUAL_STRING("Hello", buffer);
}
```

**执行顺序：**
```text
setUp()
├── test_1()
tearDown()
setUp()
├── test_2()
tearDown()
...
```

## 📝 fff Mock教程

### 1️⃣ 基本用法

```c
#include "fff.h"

/* 定义fff全局变量（每个测试文件只需一次） */
DEFINE_FFF_GLOBALS;

/* 声明fake函数 */
FAKE_VALUE_FUNC(int, calc_add, int, int);      /* 有返回值 */
FAKE_VOID_FUNC(do_something, int);              /* 无返回值 */

void setUp(void) {
    RESET_FAKE(calc_add);    /* 重置fake状态 */
    FFF_RESET_HISTORY();      /* 重置调用历史 */
}
```

### 2️⃣ 设置返回值

```c
/* 设置固定返回值 */
calc_add_fake.return_val = 42;
int result = calc_add(1, 2);  /* 返回42 */

/* 设置返回值序列 */
int return_values[] = {10, 20, 30};
SET_RETURN_SEQ(calc_add, return_values, 3);
calc_add(0, 0);  /* 返回10 */
calc_add(0, 0);  /* 返回20 */
calc_add(0, 0);  /* 返回30 */
```

### 3️⃣ 验证调用

```c
calc_add_fake.return_val = 5;
int result = calc_add(10, 20);

/* 验证调用次数 */
TEST_ASSERT_EQUAL_INT(1, calc_add_fake.call_count);

/* 验证最后一次调用的参数 */
TEST_ASSERT_EQUAL_INT(10, calc_add_fake.arg0_val);
TEST_ASSERT_EQUAL_INT(20, calc_add_fake.arg1_val);

/* 验证参数历史（多次调用） */
TEST_ASSERT_EQUAL_INT(10, calc_add_fake.arg0_history[0]);
TEST_ASSERT_EQUAL_INT(20, calc_add_fake.arg1_history[0]);
```

### 4️⃣ 自定义fake实现

```c
/* 自定义实现 */
static int custom_add(int a, int b) {
    return (a + b) * 2;  /* 返回双倍 */
}

static void test_custom_fake(void) {
    calc_add_fake.custom_fake = custom_add;

    int result = calc_add(1, 2);
    TEST_ASSERT_EQUAL_INT(6, result);  /* (1+2)*2 = 6 */
}
```

### 5️⃣ 验证调用顺序

```c
static void test_call_order(void) {
    calc_add_fake.return_val = 5;
    calc_subtract_fake.return_val = 3;

    calc_add(1, 2);
    calc_subtract(5, 2);

    /* 验证调用顺序 */
    TEST_ASSERT_EQUAL_PTR((void *)calc_add, fff.call_history[0]);
    TEST_ASSERT_EQUAL_PTR((void *)calc_subtract, fff.call_history[1]);
}
```

### 6️⃣ 与--wrap链接选项配合

要Mock真实函数（而不是fake函数），需要使用链接器的`--wrap`选项：

**Makefile配置：**
```makefile
LDFLAGS := -Wl,--wrap=calc_add -Wl,--wrap=calc_subtract
```

**测试代码：**
```c
/* fff fake声明 */
FAKE_VALUE_FUNC(int, calc_add, int, int);

/* 链接器包装函数 */
int __wrap_calc_add(int a, int b) {
    return calc_add_fake.custom_fake ?
           calc_add_fake.custom_fake(a, b) :
           calc_add(a, b);  /* 调用fff fake */
}
```

## 📊 测试输出示例

运行 `make ut_unity_run` 后的输出：

```text
========================================
Running Unity + fff Tests...
========================================

--- Running unity_test_calc ---
ut_unity_fff/src/test_calc.c:129:test_calc_add_positive_numbers:PASS
ut_unity_fff/src/test_calc.c:130:test_calc_add_negative_numbers:PASS
...
-----------------------
14 Tests 0 Failures 0 Ignored
OK

--- Running unity_test_greeting ---
ut_unity_fff/src/test_greeting.c:153:test_say_hello_with_name:PASS
...
-----------------------
13 Tests 0 Failures 0 Ignored
OK

--- Running unity_test_multi_calc (fff Mock Tests) ---
ut_unity_fff/src/test_multi_calc.c:236:test_expression_with_mocked_return_values:PASS
...
-----------------------
9 Tests 0 Failures 0 Ignored
OK
```

## 🔄 Unity vs CMocka 对比

| 特性 | Unity | CMocka |
|------|-------|--------|
| **断言风格** | `TEST_ASSERT_EQUAL_INT` | `assert_int_equal` |
| **Fixtures** | setUp/tearDown（每个测试） | group_setup/teardown + test_setup/teardown |
| **Mock框架** | 配合fff使用 | 内置will_return/mock_type |
| **测试注册** | `RUN_TEST(func)` | `cmocka_unit_test(func)` |
| **测试分组** | 手动管理 | `cmocka_run_group_tests_name` |
| **输出格式** | 自定义文本 | STDOUT/TAP/XML/Subunit |

## 📚 本项目展示的特性

| 特性 | 文件 | 说明 |
|------|------|------|
| 基本断言 | test_calc.c | TEST_ASSERT_EQUAL_INT, TEST_ASSERT_TRUE |
| 字符串断言 | test_greeting.c | TEST_ASSERT_EQUAL_STRING |
| 指针断言 | test_greeting.c | TEST_ASSERT_NOT_NULL |
| setUp/tearDown | test_greeting.c | 每个测试前后执行 |
| fff返回值 | test_multi_calc.c | return_val, SET_RETURN_SEQ |
| fff参数捕获 | test_multi_calc.c | arg0_val, arg0_history |
| fff调用计数 | test_multi_calc.c | call_count |
| fff自定义fake | test_multi_calc.c | custom_fake |
| fff调用历史 | test_multi_calc.c | fff.call_history |

## 📊 代码覆盖率

### 使用方法

```bash
# 完整流程：编译、运行测试、生成覆盖率报告
make ut_unity_cov

# 查看报告
wslview build/coverage-unity-report/index.html
```

### 报告文件

覆盖率报告生成在 `build/coverage-unity-report/` 目录：

```text
build/coverage-unity-report/
├── coverage.info              # lcov 原始数据
├── coverage_filtered.info     # 过滤后的数据
├── index.html                 # HTML 报告入口
├── sdk/src/                   # SDK 源码覆盖详情
└── ut_unity_fff/src/          # 测试代码覆盖详情
```

### 实现原理

使用 GCC 的 `--coverage` 选项编译代码，运行测试后通过 `lcov` + `genhtml` 生成 HTML 报告。

详细原理参见 [README.md](README.md#-代码覆盖率) 中的说明。

## 🔗 参考资料

- [Unity GitHub](https://github.com/ThrowTheSwitch/Unity)
- [Unity 文档](https://github.com/ThrowTheSwitch/Unity/tree/master/docs)
- [fff GitHub](https://github.com/meekrosoft/fff)
- [fff Cheat Sheet](https://github.com/meekrosoft/fff#cheat-sheet)
