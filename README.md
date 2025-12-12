# cmocka使用

官网：https://cmocka.org/

API文档：https://api.cmocka.org/index.html

## 介绍

“只做一件事，但做得好”，是CMocka的设计哲学。

他是`Check`单元测试框架的一个分支，为C代码单元测试提供了一种简单、稳定的方法。

## 安装

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

# 编译并安装
# -DCMAKE_INSTALL_PREFIX 指定安装位置
# -DCMAKE_BUILD_TYPE=Debug 建议开启调试模式方便测试
cmake -DCMAKE_INSTALL_PREFIX=./cmocka-install -DCMAKE_BUILD_TYPE=Debug ..
make
make install
```

执行完成后，在`./build/cmocka-install`目录中生成头文件（`include`）和库文件（`lib`）

*但是只看到了动态库，没有看到静态库；`lib`目录中的`cmake`和`pkconfig`可以删除*

将`cmocka-install`目录放到自己的项目目录中，准备使用

## 使用

### 项目示例

```text
.
├── application
│   └── main.c
├── Makefile
├── sdk
│   ├── include
│   └── src
└── ut
    ├── cmocka-install
    └── src

```

示例介绍：

`SDK`提供函数接口给应用层（`application`）使用，生成可执行文件

项目目标：

使用单元测试工具`cmocka`，测试`sdk`提供的接口






