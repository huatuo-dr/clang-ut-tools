# Main Makefile for CMocka Project

# Compiler and tools
CC := gcc
AR := ar
MKDIR := mkdir -p
RM := rm -rf

# Build flags
CFLAGS := -Wall -Wextra -g
ARFLAGS := rcs

# Directories
OUTPUT_DIR := output
BUILD_DIR := build
DIST_DIR := dist
SDK_OUTPUT_DIR := $(OUTPUT_DIR)/sdk
APP_OUTPUT_DIR := $(OUTPUT_DIR)/application
UT_OUTPUT_DIR := $(OUTPUT_DIR)/ut

# Install directories
SDK_INSTALL_DIR := $(BUILD_DIR)/sdk
SDK_INSTALL_INC_DIR := $(SDK_INSTALL_DIR)/include
SDK_INSTALL_LIB_DIR := $(SDK_INSTALL_DIR)/lib

# Include sub-makefiles
include sdk/sdk.mk
include application/application.mk
include ut/ut.mk

# Default target
.PHONY: all
all: sdk

# Clean target
.PHONY: clean
clean:
	$(RM) $(OUTPUT_DIR) $(BUILD_DIR) $(DIST_DIR)

# Display help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make sdk         - Build SDK library (libsdk.a)"
	@echo "  make sdk_install - Build and install SDK to build directory"
	@echo "  make app         - Build application executable"
	@echo "  make run         - Build and run the application"
	@echo "  make ut          - Build and run unit tests"
	@echo "  make ut_build    - Build unit tests only (without running)"
	@echo "  make clean       - Remove all build artifacts"
	@echo "  make help        - Display this help message"