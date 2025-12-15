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
UT_OUTPUT_DIR := $(OUTPUT_DIR)/ut_cmocka

# Install directories
SDK_INSTALL_DIR := $(BUILD_DIR)/sdk
SDK_INSTALL_INC_DIR := $(SDK_INSTALL_DIR)/include
SDK_INSTALL_LIB_DIR := $(SDK_INSTALL_DIR)/lib

# Include sub-makefiles
include sdk/sdk.mk
include application/application.mk
include ut_cmocka/ut.mk
include ut_cmocka/ut_cov.mk
include ut_unity_fff/ut.mk
include ut_unity_fff/ut_cov.mk
include ut_gtest_gmock/ut.mk
include ut_gtest_gmock/ut_cov.mk
include ut_gtest_mockcpp/ut.mk
include ut_gtest_mockcpp/ut_cov.mk

# Default target
.PHONY: all
all: sdk

# Run all unit tests (all frameworks)
.PHONY: ut
ut: ut_cmocka ut_unity ut_gtest ut_gtest_mockcpp
	@echo ""
	@echo "========================================"
	@echo "All Unit Tests Completed!"
	@echo "========================================"

# Run all coverage tests (all frameworks)
.PHONY: ut_cov
ut_cov: ut_cmocka_cov ut_unity_cov ut_gtest_cov ut_gtest_mockcpp_cov
	@echo ""
	@echo "========================================"
	@echo "All Coverage Reports Generated!"
	@echo "========================================"

# Clean target
.PHONY: clean
clean:
	$(RM) $(OUTPUT_DIR) $(BUILD_DIR) $(DIST_DIR)

# Display help
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  make sdk           - Build SDK library (libsdk.a)"
	@echo "  make sdk_install   - Build and install SDK to build directory"
	@echo "  make app           - Build application executable"
	@echo "  make run           - Build and run the application"
	@echo ""
	@echo "  All frameworks:"
	@echo "  make ut            - Run all unit tests (all frameworks)"
	@echo "  make ut_cov        - Run all coverage tests (all frameworks)"
	@echo ""
	@echo "  CMocka unit tests:"
	@echo "  make ut_cmocka      - Build and run CMocka unit tests"
	@echo "  make ut_cmocka_build - Build CMocka tests only (without running)"
	@echo "  make ut_cmocka_run  - Run CMocka tests (terminal output)"
	@echo "  make ut_cmocka_report - Generate CMocka test reports"
	@echo ""
	@echo "  CMocka code coverage:"
	@echo "  make ut_cmocka_cov  - Run tests and generate coverage report"
	@echo "  make ut_cmocka_cov_run - Run coverage tests only"
	@echo "  make ut_cmocka_cov_report - Generate HTML coverage report"
	@echo "  make clean-cmocka-cov - Clean coverage artifacts"
	@echo ""
	@echo "  Unity + fff unit tests:"
	@echo "  make ut_unity      - Build and run Unity + fff tests"
	@echo "  make ut_unity_build- Build Unity tests only (without running)"
	@echo "  make ut_unity_run  - Run Unity tests (terminal output)"
	@echo "  make ut_unity_report - Generate Unity test reports"
	@echo ""
	@echo "  Unity code coverage:"
	@echo "  make ut_unity_cov  - Run Unity tests and generate coverage report"
	@echo "  make ut_unity_cov_run - Run Unity coverage tests only"
	@echo "  make ut_unity_cov_report - Generate Unity HTML coverage report"
	@echo "  make clean-unity-cov - Clean Unity coverage artifacts"
	@echo ""
	@echo "  GoogleTest + GMock unit tests:"
	@echo "  make ut_gtest      - Build and run GoogleTest + GMock tests"
	@echo "  make ut_gtest_build- Build GoogleTest tests only (without running)"
	@echo "  make ut_gtest_run  - Run GoogleTest tests (terminal output)"
	@echo "  make ut_gtest_report - Generate GoogleTest test reports"
	@echo ""
	@echo "  GoogleTest code coverage:"
	@echo "  make ut_gtest_cov  - Run GoogleTest tests and generate coverage report"
	@echo "  make ut_gtest_cov_run - Run GoogleTest coverage tests only"
	@echo "  make ut_gtest_cov_report - Generate GoogleTest HTML coverage report"
	@echo "  make clean-gtest-cov - Clean GoogleTest coverage artifacts"
	@echo ""
	@echo "  GoogleTest + mockcpp unit tests:"
	@echo "  make ut_gtest_mockcpp - Build and run GoogleTest + mockcpp tests"
	@echo "  make ut_gtest_mockcpp_build - Build tests only (without running)"
	@echo "  make ut_gtest_mockcpp_run - Run tests (terminal output)"
	@echo "  make ut_gtest_mockcpp_report - Generate test reports"
	@echo ""
	@echo "  GoogleTest + mockcpp code coverage:"
	@echo "  make ut_gtest_mockcpp_cov - Run tests and generate coverage report"
	@echo "  make ut_gtest_mockcpp_cov_run - Run coverage tests only"
	@echo "  make ut_gtest_mockcpp_cov_report - Generate HTML coverage report"
	@echo "  make clean-gtest-mockcpp-cov - Clean coverage artifacts"
	@echo ""
	@echo "  make clean         - Remove all build artifacts"
	@echo "  make help          - Display this help message"