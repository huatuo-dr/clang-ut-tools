# Coverage build rules for GoogleTest + MockCpp unit tests
# Uses gcov + lcov + genhtml to generate HTML coverage reports

# Coverage directories
GTEST_MOCKCPP_COV_OUTPUT_DIR := $(OUTPUT_DIR)/coverage_gtest_mockcpp
GTEST_MOCKCPP_COV_SDK_OUTPUT_DIR := $(GTEST_MOCKCPP_COV_OUTPUT_DIR)/sdk
GTEST_MOCKCPP_COV_UT_OUTPUT_DIR := $(GTEST_MOCKCPP_COV_OUTPUT_DIR)/ut
GTEST_MOCKCPP_COV_REPORT_DIR := $(BUILD_DIR)/coverage-gtest-mockcpp-report

# Coverage flags
GTEST_MOCKCPP_COV_CFLAGS := $(CFLAGS) --coverage -fprofile-arcs -ftest-coverage
GTEST_MOCKCPP_COV_CXXFLAGS := $(CXXFLAGS) --coverage -fprofile-arcs -ftest-coverage
GTEST_MOCKCPP_COV_LDFLAGS := --coverage

# Coverage SDK object files
GTEST_MOCKCPP_COV_SDK_SRCS := $(wildcard sdk/src/*.c)
GTEST_MOCKCPP_COV_SDK_OBJS := $(patsubst sdk/src/%.c, $(GTEST_MOCKCPP_COV_SDK_OUTPUT_DIR)/%.o, $(GTEST_MOCKCPP_COV_SDK_SRCS))

# Coverage UT object files
GTEST_MOCKCPP_COV_UT_SRCS := $(wildcard $(GTEST_MOCKCPP_SRC_DIR)/*.cpp)
GTEST_MOCKCPP_COV_UT_OBJS := $(patsubst $(GTEST_MOCKCPP_SRC_DIR)/%.cpp, $(GTEST_MOCKCPP_COV_UT_OUTPUT_DIR)/%.o, $(GTEST_MOCKCPP_COV_UT_SRCS))

# Coverage test executables
GTEST_MOCKCPP_COV_TEST_MULTI_CALC := $(GTEST_MOCKCPP_COV_OUTPUT_DIR)/gtest_mockcpp_test_multi_calc

# Coverage SDK library
GTEST_MOCKCPP_COV_SDK_LIB := $(GTEST_MOCKCPP_COV_OUTPUT_DIR)/libsdk_cov.a

# UT specific flags for coverage build
# Note: mockcpp uses runtime API hooking, no --wrap needed
GTEST_MOCKCPP_COV_UT_CXXFLAGS := $(GTEST_MOCKCPP_COV_CXXFLAGS) -Isdk/include -I$(GTEST_MOCKCPP_GTEST_INC) -I$(GTEST_MOCKCPP_MOCKCPP_INC)
GTEST_MOCKCPP_COV_UT_LDFLAGS := $(GTEST_MOCKCPP_COV_LDFLAGS) -L$(GTEST_MOCKCPP_COV_OUTPUT_DIR) -L$(GTEST_MOCKCPP_GTEST_LIB) -L$(GTEST_MOCKCPP_MOCKCPP_LIB) \
    -lsdk_cov -lgtest -lmockcpp -lpthread

# Build and run coverage tests, then generate report
.PHONY: ut_gtest_mockcpp_cov
ut_gtest_mockcpp_cov: ut_gtest_mockcpp_cov_run ut_gtest_mockcpp_cov_report
	@echo ""
	@echo "========================================"
	@echo "GoogleTest + MockCpp Coverage Report Generated!"
	@echo "  HTML: $(GTEST_MOCKCPP_COV_REPORT_DIR)/index.html"
	@echo "========================================"

# Run coverage tests
.PHONY: ut_gtest_mockcpp_cov_run
ut_gtest_mockcpp_cov_run: ut_gtest_mockcpp_cov_build
	@echo ""
	@echo "========================================"
	@echo "Running GoogleTest + MockCpp Coverage Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running gtest_mockcpp_test_multi_calc (coverage) ---"
	@$(GTEST_MOCKCPP_COV_TEST_MULTI_CALC)

# Generate coverage report using lcov and genhtml
.PHONY: ut_gtest_mockcpp_cov_report
ut_gtest_mockcpp_cov_report: ut_gtest_mockcpp_cov_run
	@echo ""
	@echo "========================================"
	@echo "Generating GoogleTest + MockCpp Coverage Report..."
	@echo "========================================"
	@$(MKDIR) $(GTEST_MOCKCPP_COV_REPORT_DIR)
	@echo "Collecting coverage data..."
	@lcov --capture --directory $(GTEST_MOCKCPP_COV_OUTPUT_DIR) --output-file $(GTEST_MOCKCPP_COV_REPORT_DIR)/coverage.info
	@echo "Filtering out system headers and libraries..."
	@lcov --remove $(GTEST_MOCKCPP_COV_REPORT_DIR)/coverage.info '/usr/*' '*/gtest-install/*' '*/mockcpp-install/*' --output-file $(GTEST_MOCKCPP_COV_REPORT_DIR)/coverage_filtered.info
	@echo "Generating HTML report..."
	@genhtml $(GTEST_MOCKCPP_COV_REPORT_DIR)/coverage_filtered.info --output-directory $(GTEST_MOCKCPP_COV_REPORT_DIR) --title "GoogleTest + MockCpp SDK Coverage Report" --legend
	@echo ""
	@echo "Coverage report generated:"
	@echo "  $(GTEST_MOCKCPP_COV_REPORT_DIR)/index.html"

# Build coverage SDK library
$(GTEST_MOCKCPP_COV_SDK_LIB): $(GTEST_MOCKCPP_COV_SDK_OBJS)
	@echo "Building coverage SDK library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Compile SDK source files with coverage (C files)
$(GTEST_MOCKCPP_COV_SDK_OUTPUT_DIR)/%.o: sdk/src/%.c
	@echo "Compiling (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(GTEST_MOCKCPP_COV_CFLAGS) -Isdk/include -c $< -o $@

# Build coverage test executables
.PHONY: ut_gtest_mockcpp_cov_build
ut_gtest_mockcpp_cov_build: $(GTEST_MOCKCPP_COV_SDK_LIB) $(GTEST_MOCKCPP_COV_TEST_MULTI_CALC)
	@echo "GoogleTest + MockCpp coverage test executables built successfully"

# Build coverage gtest_mockcpp_test_multi_calc
$(GTEST_MOCKCPP_COV_TEST_MULTI_CALC): $(GTEST_MOCKCPP_COV_UT_OUTPUT_DIR)/test_multi_calc.o $(GTEST_MOCKCPP_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CXX) $< -o $@ $(GTEST_MOCKCPP_COV_UT_LDFLAGS)

# Compile UT source files with coverage (C++ files)
$(GTEST_MOCKCPP_COV_UT_OUTPUT_DIR)/%.o: $(GTEST_MOCKCPP_SRC_DIR)/%.cpp
	@echo "Compiling test (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CXX) $(GTEST_MOCKCPP_COV_UT_CXXFLAGS) -c $< -o $@

# Clean GoogleTest + MockCpp coverage artifacts
.PHONY: clean-gtest-mockcpp-cov
clean-gtest-mockcpp-cov:
	$(RM) $(GTEST_MOCKCPP_COV_OUTPUT_DIR) $(GTEST_MOCKCPP_COV_REPORT_DIR)
	@find . -name "*.gcno" -path "*/coverage_gtest_mockcpp/*" -delete 2>/dev/null || true
	@find . -name "*.gcda" -path "*/coverage_gtest_mockcpp/*" -delete 2>/dev/null || true
	@find . -name "*.gcov" -path "*/coverage_gtest_mockcpp/*" -delete 2>/dev/null || true
