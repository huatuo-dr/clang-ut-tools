# cmocka使用

官网：https://cmocka.org/

API文档：https://api.cmocka.org/index.html

## 📖 介绍

"只做一件事，但做得好"，是CMocka的设计哲学。

它是`Check`单元测试框架的一个分支，为C代码单元测试提供了一种简单、稳定的方法。

### ✨ 主要特性

- 支持Mock对象（模拟对象）
- 支持Test Fixtures（测试夹具：setup和teardown）
- 仅依赖标准C库
- 信号异常处理（SIGSEGV, SIGILL等）
- 内存泄漏检测
- 丰富的断言宏
- 多种输出格式（stdout, TAP, JUnit XML, Subunit）

## 🔧 安装

### 方法一：全局安装

可以使用命令：

```shell
sudo apt update
sudo apt install cmocka
```

### 方法二：编译源码

可以手动从官网上（https://cmocka.org/files/）下载源码，进行编译

或直接使用下面的命令：

```shell
# 下载并解压源码（以2.0.0为例）
wget https://cmocka.org/files/2.0/cmocka-2.0.0.tar.xz
tar -xvf cmocka-2.0.0.tar.xz
cd cmocka-2.0.0

# 创建build目录
mkdir build && cd build

# 编译并安装
# -DCMAKE_INSTALL_PREFIX 指定安装位置
# -DCMAKE_BUILD_TYPE=Debug 建议开启调试模式方便测试
cmake -DCMAKE_INSTALL_PREFIX=./cmocka-install -DCMAKE_BUILD_TYPE=Debug ..
make
make install
```

执行完成后，在`./build/cmocka-install`目录中生成头文件（`include`）和库文件（`lib`）

*但是只看到了动态库，没有看到静态库；`lib`目录中的`cmake`和`pkgconfig`可以删除*

将`cmocka-install`目录放到自己的项目目录中，准备使用

## 📁 项目结构

```text
.
├── Makefile                    # 主Makefile
├── README.md                   # 本文档
├── .gitignore                  # Git忽略文件
├── 3rdparty/                   # 第三方库
│   └── cmocka-2.0.0/           # cmocka源码
├── sdk/                        # SDK库（被测代码）
│   ├── sdk.mk                  # SDK编译规则
│   ├── include/                # 头文件
│   │   ├── calc.h              # 计算器模块
│   │   └── greeting.h          # 问候模块
│   └── src/                    # 源代码
│       ├── calc.c
│       └── greeting.c
├── application/                # 应用程序
│   ├── application.mk          # 应用编译规则
│   └── main.c                  # 主程序
├── ut/                         # 单元测试
│   ├── ut.mk                   # 测试编译规则
│   ├── cmocka-install/         # cmocka库（已编译）
│   │   ├── include/
│   │   └── lib/
│   └── src/                    # 测试源码
│       ├── test_calc.c         # calc模块测试
│       └── test_greeting.c     # greeting模块测试
├── output/                     # 编译中间文件（自动生成）
├── build/                      # SDK安装目录（自动生成）
└── dist/                       # 可执行文件输出（自动生成）
```

## 🚀 构建命令

```shell
make help          # 查看所有可用命令
make sdk           # 编译SDK库
make sdk_install   # 安装SDK到build目录
make app           # 编译应用程序
make run           # 运行应用程序
make ut            # 编译并运行单元测试
make ut_build      # 仅编译单元测试（不运行）
make clean         # 清理所有编译产物
```

## 📝 cmocka基础教程

### 1️⃣ 最简单的测试用例

```c
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

// 测试函数，参数固定为 void **state
static void test_example(void **state) {
    (void)state;  // 未使用时避免编译警告
    assert_true(1 == 1);
}

int main(void) {
    // 定义测试用例数组
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_example),
    };

    // 运行测试
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

**📌 注意事项：**
- 头文件的包含顺序很重要，必须按照上述顺序
- 每个测试函数的签名必须是 `void test_xxx(void **state)`
- `cmocka_unit_test()` 宏将函数注册为测试用例

### 2️⃣ 常用断言宏

cmocka提供了丰富的断言宏，测试失败时会打印详细的错误信息：

#### 布尔断言

```c
assert_true(expression);           // 表达式为真
assert_false(expression);          // 表达式为假
```

#### 整数断言

```c
assert_int_equal(a, b);            // a == b
assert_int_not_equal(a, b);        // a != b
```

#### 指针断言

```c
assert_null(ptr);                  // ptr == NULL
assert_non_null(ptr);              // ptr != NULL
assert_ptr_equal(a, b);            // 指针相等
assert_ptr_not_equal(a, b);        // 指针不等
```

#### 内存断言

```c
assert_memory_equal(a, b, size);   // 内存内容相等
assert_memory_not_equal(a, b, size);
```

#### 字符串断言

```c
assert_string_equal(a, b);         // 字符串相等
assert_string_not_equal(a, b);     // 字符串不等
```

#### 范围断言

```c
assert_in_range(value, min, max);      // min <= value <= max
assert_not_in_range(value, min, max);  // value < min 或 value > max
```

### 3️⃣ 测试分组

可以将相关的测试用例分组，便于组织和管理：

```c
int main(void) {
    // 加法测试组
    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test(test_add_positive),
        cmocka_unit_test(test_add_negative),
        cmocka_unit_test(test_add_zero),
    };

    // 减法测试组
    const struct CMUnitTest subtract_tests[] = {
        cmocka_unit_test(test_subtract_positive),
        cmocka_unit_test(test_subtract_negative),
    };

    int result = 0;

    // 分别运行每个测试组，并指定组名
    result += cmocka_run_group_tests_name("add tests", add_tests, NULL, NULL);
    result += cmocka_run_group_tests_name("subtract tests", subtract_tests, NULL, NULL);

    return result;
}
```

### 4️⃣ Test Fixtures（测试夹具）

Fixtures用于在测试前后执行初始化和清理工作。

#### 4.1 组级别的 Setup/Teardown

在**一组测试**开始前和结束后各执行一次：

```c
// 组初始化 - 在所有测试开始前执行一次
static int group_setup(void **state) {
    // 分配资源、初始化环境
    struct my_context *ctx = malloc(sizeof(struct my_context));
    if (ctx == NULL) return -1;

    *state = ctx;  // 通过state传递给测试函数
    return 0;      // 返回0表示成功
}

// 组清理 - 在所有测试结束后执行一次
static int group_teardown(void **state) {
    struct my_context *ctx = *state;
    free(ctx);     // 释放资源
    return 0;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_1),
        cmocka_unit_test(test_2),
    };

    // 传入 group_setup 和 group_teardown
    return cmocka_run_group_tests(tests, group_setup, group_teardown);
}
```

#### 4.2 测试级别的 Setup/Teardown

在**每个测试**开始前和结束后都执行：

```c
// 每个测试前执行
static int test_setup(void **state) {
    // 初始化测试数据
    return 0;
}

// 每个测试后执行
static int test_teardown(void **state) {
    // 清理测试数据
    return 0;
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // 使用 cmocka_unit_test_setup_teardown 指定每个测试的setup/teardown
        cmocka_unit_test_setup_teardown(test_1, test_setup, test_teardown),
        cmocka_unit_test_setup_teardown(test_2, test_setup, test_teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

#### 4.3 执行顺序

```text
group_setup()           # 组初始化（1次）
├── test_setup()        # 测试1初始化
│   └── test_1()        # 测试1执行
├── test_teardown()     # 测试1清理
├── test_setup()        # 测试2初始化
│   └── test_2()        # 测试2执行
├── test_teardown()     # 测试2清理
group_teardown()        # 组清理（1次）
```

### 5️⃣ 参数化测试

使用 `cmocka_unit_test_prestate` 可以为同一个测试函数传入不同的测试数据：

```c
// 测试数据结构
struct test_data {
    int a;
    int b;
    int expected;
};

// 测试函数
static void test_add(void **state) {
    struct test_data *data = (struct test_data *)*state;
    assert_int_equal(calc_add(data->a, data->b), data->expected);
}

// 多组测试数据
static struct test_data test_cases[] = {
    {1, 1, 2},
    {0, 0, 0},
    {-1, 1, 0},
    {100, 200, 300},
};

int main(void) {
    const struct CMUnitTest tests[] = {
        // 同一个测试函数，不同的测试数据
        cmocka_unit_test_prestate(test_add, &test_cases[0]),
        cmocka_unit_test_prestate(test_add, &test_cases[1]),
        cmocka_unit_test_prestate(test_add, &test_cases[2]),
        cmocka_unit_test_prestate(test_add, &test_cases[3]),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### 6️⃣ Mock测试（函数模拟）

Mock是cmocka的核心特性，用于**模拟外部依赖**，让测试只关注被测代码本身。

#### 6.1 基本原理

假设有函数A依赖函数B，我们想测试A但不想执行真实的B：

```text
┌─────────────────────────────────────────────────────────┐
│   被测函数 A                                            │
│       │                                                 │
│       ▼                                                 │
│   调用函数 B  ──── 使用 --wrap ────→  __wrap_B (Mock)   │
│                                        返回预设值       │
└─────────────────────────────────────────────────────────┘
```

#### 6.2 实现步骤

**步骤1：编写Mock函数**

```c
// Mock函数名必须是 __wrap_ + 原函数名
int __wrap_calc_add(int a, int b) {
    (void)a;
    (void)b;
    // 返回will_return()预设的值
    return mock_type(int);
}
```

**步骤2：在测试中设置Mock返回值**

```c
static void test_with_mock(void **state) {
    (void)state;

    // 按调用顺序设置Mock返回值
    will_return(__wrap_calc_add, 100);      // 第1次调用返回100
    will_return(__wrap_calc_subtract, 50);  // 第2次调用返回50

    // 调用被测函数（内部会调用calc_add和calc_subtract）
    int result = my_function_under_test(1, 2, 3, 4);

    // 验证结果
    assert_int_equal(result, expected_value);
}
```

**步骤3：编译时添加--wrap选项**

```makefile
# 关键：使用 -Wl,--wrap=函数名 替换函数
LDFLAGS := -Wl,--wrap=calc_add -Wl,--wrap=calc_subtract -lcmocka

test_xxx: test_xxx.c
    $(CC) $< -o $@ $(LDFLAGS)
```

#### 6.3 Mock API汇总

```c
// 设置Mock返回值
will_return(function, value);           // 设置一次返回值
will_return_always(function, value);    // 始终返回该值
will_return_count(function, value, n);  // 返回n次

// 在Mock函数中获取预设值
mock();                                 // 获取预设值（通用）
mock_type(type);                        // 获取并转换为指定类型
mock_ptr_type(type);                    // 获取指针类型
```

#### 6.4 本项目Mock示例

本项目中 `test_multi_calc.c` 展示了完整的Mock测试：

- **被测函数**: `multi_calc_expression()`, `multi_calc_average()`
- **被Mock函数**: `calc_add()`, `calc_subtract()`, `calc_multiply()`, `calc_divide()`

```c
// 测试 (a + b) * (c - d) = (2 + 3) * (10 - 4) = 5 * 6 = 30
static void test_expression_normal(void **state) {
    (void)state;

    // 按调用顺序设置Mock返回值
    will_return(__wrap_calc_add, 5);       // calc_add(2, 3) -> 5
    will_return(__wrap_calc_subtract, 6);  // calc_subtract(10, 4) -> 6
    will_return(__wrap_calc_multiply, 30); // calc_multiply(5, 6) -> 30

    int result = multi_calc_expression(2, 3, 10, 4);

    assert_int_equal(result, 30);
}
```

### 7️⃣ 混合测试（Real + Mock）

在同一个测试文件中，可以灵活地在**真实函数**和**Mock函数**之间切换。

#### 7.1 实现原理

使用 `-Wl,--wrap=func` 链接选项时，**GCC链接器会自动生成两个符号**：

| 符号 | 来源 | 说明 |
|------|------|------|
| `__wrap_func` | 需要你定义 | 替换原函数的包装函数 |
| `__real_func` | **链接器自动生成** | 指向原始函数实现，无需手动定义 |

因此，代码中只需用 `extern` 声明 `__real_xxx`，链接器会自动解析：

```c
// 声明原始函数（链接器自动提供，无需定义函数体）
extern int __real_calc_add(int a, int b);

// Mock控制标志
static bool mock_calc_add = true;

// __wrap函数中根据标志选择调用方式
int __wrap_calc_add(int a, int b) {
    if (mock_calc_add) {
        return mock_type(int);  // 使用Mock值
    } else {
        return __real_calc_add(a, b);  // 调用真实函数
    }
}
```

#### 7.2 辅助函数

```c
// 启用所有Mock
static void enable_all_mocks(void) {
    mock_calc_add = true;
    mock_calc_subtract = true;
    mock_calc_multiply = true;
    mock_calc_divide = true;
}

// 禁用所有Mock（使用真实函数）
static void disable_all_mocks(void) {
    mock_calc_add = false;
    mock_calc_subtract = false;
    mock_calc_multiply = false;
    mock_calc_divide = false;
}
```

#### 7.3 测试用例示例

```c
// 测试1：全部使用真实函数
static void test_expression_real_all(void **state) {
    (void)state;
    disable_all_mocks();  // 关闭所有Mock

    // (2 + 3) * (10 - 4) = 5 * 6 = 30
    int result = multi_calc_expression(2, 3, 10, 4);
    assert_int_equal(result, 30);
}

// 测试2：部分Mock（仅Mock乘法）
static void test_expression_partial_mock(void **state) {
    (void)state;

    mock_calc_add = false;       // 使用真实加法
    mock_calc_subtract = false;  // 使用真实减法
    mock_calc_multiply = true;   // Mock乘法

    // calc_add(2, 3) = 5 (真实)
    // calc_subtract(10, 4) = 6 (真实)
    // calc_multiply(5, 6) = Mock返回999
    will_return(__wrap_calc_multiply, 999);

    int result = multi_calc_expression(2, 3, 10, 4);
    assert_int_equal(result, 999);  // 验证Mock生效
}

// 测试3：对比Mock和真实结果
static void test_compare_mock_vs_real(void **state) {
    (void)state;

    // 先用Mock测试
    enable_all_mocks();
    will_return(__wrap_calc_add, 100);
    will_return(__wrap_calc_add, 200);
    will_return(__wrap_calc_divide, 66);
    int mock_result = multi_calc_average(1, 2, 3);

    // 再用真实函数测试
    disable_all_mocks();
    int real_result = multi_calc_average(1, 2, 3);

    // 验证结果不同
    assert_int_equal(mock_result, 66);  // Mock结果
    assert_int_equal(real_result, 2);   // 真实结果: (1+2+3)/3 = 2
}
```

#### 7.4 使用场景

| 场景 | 配置 | 说明 |
|------|------|------|
| 纯Mock测试 | enable_all_mocks() | 完全控制依赖函数返回值 |
| 纯真实测试 | disable_all_mocks() | 验证真实实现的正确性 |
| 部分Mock | 单独设置各标志 | 测试特定依赖的异常行为 |
| 对比测试 | 切换Mock状态 | 验证Mock行为与真实行为的差异 |

## 🔨 编译测试程序

### Makefile示例

```makefile
# cmocka路径
CMOCKA_INC := ut/cmocka-install/include
CMOCKA_LIB := ut/cmocka-install/lib

# 编译选项
CFLAGS := -Wall -Wextra -g -I$(CMOCKA_INC) -I<你的头文件目录>
LDFLAGS := -L$(CMOCKA_LIB) -L<你的库目录> -l<你的库名> -lcmocka

# 编译测试程序
test_xxx: test_xxx.c
    $(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 运行测试（需要设置动态库路径）
run_test: test_xxx
    LD_LIBRARY_PATH=$(CMOCKA_LIB):$$LD_LIBRARY_PATH ./test_xxx
```

### 编译命令说明

- `-I$(CMOCKA_INC)`: 指定cmocka头文件路径
- `-L$(CMOCKA_LIB)`: 指定cmocka库文件路径
- `-lcmocka`: 链接cmocka库
- `LD_LIBRARY_PATH`: 运行时动态库搜索路径

## 📊 测试输出示例

运行 `make ut` 后的输出：

```text
========================================
Running Unit Tests...
========================================

--- Running test_calc ---

========== CALC MODULE UNIT TESTS ==========

[==========] calc_add tests: Running 4 test(s).
[ RUN      ] test_calc_add_positive_numbers
[       OK ] test_calc_add_positive_numbers
[ RUN      ] test_calc_add_negative_numbers
[       OK ] test_calc_add_negative_numbers
[ RUN      ] test_calc_add_mixed_numbers
[       OK ] test_calc_add_mixed_numbers
[ RUN      ] test_calc_add_zero
[       OK ] test_calc_add_zero
[==========] calc_add tests: 4 test(s) run.
[  PASSED  ] 4 test(s).

...

========================================
All Unit Tests Completed!
========================================
```

## 📚 本项目展示的cmocka特性

| 特性 | 文件 | 说明 |
|------|------|------|
| 基本断言 | test_calc.c | assert_int_equal, assert_true |
| 字符串断言 | test_greeting.c | assert_string_equal |
| 指针断言 | test_greeting.c | assert_non_null |
| 测试分组 | 所有测试文件 | cmocka_run_group_tests_name |
| 参数化测试 | test_calc.c | cmocka_unit_test_prestate |
| Group Fixtures | test_greeting.c | group_setup, group_teardown |
| Test Fixtures | test_greeting.c | test_setup, test_teardown |
| **Mock测试** | test_multi_calc.c | will_return, mock_type, __wrap_ |
| **混合测试** | test_multi_calc.c | __real_xxx, 动态切换Mock/真实函数 |

## 🔗 参考资料

- [cmocka官网](https://cmocka.org/)
- [cmocka API文档](https://api.cmocka.org/index.html)
- [cmocka GitLab](https://gitlab.com/cmocka/cmocka)
