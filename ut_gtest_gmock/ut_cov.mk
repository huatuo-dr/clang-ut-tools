# Coverage build rules for GoogleTest + GMock unit tests
# Uses gcov + lcov + genhtml to generate HTML coverage reports

# Coverage directories
GTEST_COV_OUTPUT_DIR := $(OUTPUT_DIR)/coverage_gtest
GTEST_COV_SDK_OUTPUT_DIR := $(GTEST_COV_OUTPUT_DIR)/sdk
GTEST_COV_UT_OUTPUT_DIR := $(GTEST_COV_OUTPUT_DIR)/ut
GTEST_COV_REPORT_DIR := $(BUILD_DIR)/coverage-gtest-report

# Coverage flags
GTEST_COV_CFLAGS := $(CFLAGS) --coverage -fprofile-arcs -ftest-coverage
GTEST_COV_CXXFLAGS := $(CXXFLAGS) --coverage -fprofile-arcs -ftest-coverage
GTEST_COV_LDFLAGS := --coverage

# Coverage SDK object files
GTEST_COV_SDK_SRCS := $(wildcard sdk/src/*.c)
GTEST_COV_SDK_OBJS := $(patsubst sdk/src/%.c, $(GTEST_COV_SDK_OUTPUT_DIR)/%.o, $(GTEST_COV_SDK_SRCS))

# Coverage UT object files
GTEST_COV_UT_SRCS := $(wildcard $(GTEST_SRC_DIR)/*.cpp)
GTEST_COV_UT_OBJS := $(patsubst $(GTEST_SRC_DIR)/%.cpp, $(GTEST_COV_UT_OUTPUT_DIR)/%.o, $(GTEST_COV_UT_SRCS))

# Coverage test executables
GTEST_COV_TEST_CALC := $(GTEST_COV_OUTPUT_DIR)/gtest_test_calc
GTEST_COV_TEST_GREETING := $(GTEST_COV_OUTPUT_DIR)/gtest_test_greeting
GTEST_COV_TEST_MULTI_CALC := $(GTEST_COV_OUTPUT_DIR)/gtest_test_multi_calc

# Coverage SDK library
GTEST_COV_SDK_LIB := $(GTEST_COV_OUTPUT_DIR)/libsdk_cov.a

# UT specific flags for coverage build
GTEST_COV_UT_CXXFLAGS := $(GTEST_COV_CXXFLAGS) -Isdk/include -I$(GTEST_INC_DIR)
GTEST_COV_UT_LDFLAGS := $(GTEST_COV_LDFLAGS) -L$(GTEST_COV_OUTPUT_DIR) -L$(GTEST_LIB_DIR) -lsdk_cov -lgtest -lgmock -lpthread

# Mock test specific LDFLAGS for coverage
GTEST_COV_MOCK_LDFLAGS := $(GTEST_COV_UT_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run coverage tests, then generate report
.PHONY: ut_gtest_cov
ut_gtest_cov: ut_gtest_cov_run ut_gtest_cov_report
	@echo ""
	@echo "========================================"
	@echo "GoogleTest Coverage Report Generated!"
	@echo "  HTML: $(GTEST_COV_REPORT_DIR)/index.html"
	@echo "========================================"

# Run coverage tests
.PHONY: ut_gtest_cov_run
ut_gtest_cov_run: ut_gtest_cov_build
	@echo ""
	@echo "========================================"
	@echo "Running GoogleTest Coverage Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running gtest_test_calc (coverage) ---"
	@$(GTEST_COV_TEST_CALC)
	@echo ""
	@echo "--- Running gtest_test_greeting (coverage) ---"
	@$(GTEST_COV_TEST_GREETING)
	@echo ""
	@echo "--- Running gtest_test_multi_calc (coverage) ---"
	@$(GTEST_COV_TEST_MULTI_CALC)

# Generate coverage report using lcov and genhtml
.PHONY: ut_gtest_cov_report
ut_gtest_cov_report: ut_gtest_cov_run
	@echo ""
	@echo "========================================"
	@echo "Generating GoogleTest Coverage Report..."
	@echo "========================================"
	@$(MKDIR) $(GTEST_COV_REPORT_DIR)
	@echo "Collecting coverage data..."
	@lcov --capture --directory $(GTEST_COV_OUTPUT_DIR) --output-file $(GTEST_COV_REPORT_DIR)/coverage.info
	@echo "Filtering out system headers and gtest library..."
	@lcov --remove $(GTEST_COV_REPORT_DIR)/coverage.info '/usr/*' '*/gtest-install/*' --output-file $(GTEST_COV_REPORT_DIR)/coverage_filtered.info
	@echo "Generating HTML report..."
	@genhtml $(GTEST_COV_REPORT_DIR)/coverage_filtered.info --output-directory $(GTEST_COV_REPORT_DIR) --title "GoogleTest SDK Coverage Report" --legend
	@echo ""
	@echo "Coverage report generated:"
	@echo "  $(GTEST_COV_REPORT_DIR)/index.html"

# Build coverage SDK library
$(GTEST_COV_SDK_LIB): $(GTEST_COV_SDK_OBJS)
	@echo "Building coverage SDK library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Compile SDK source files with coverage (C files)
$(GTEST_COV_SDK_OUTPUT_DIR)/%.o: sdk/src/%.c
	@echo "Compiling (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(GTEST_COV_CFLAGS) -Isdk/include -c $< -o $@

# Build coverage test executables
.PHONY: ut_gtest_cov_build
ut_gtest_cov_build: $(GTEST_COV_SDK_LIB) $(GTEST_COV_TEST_CALC) $(GTEST_COV_TEST_GREETING) $(GTEST_COV_TEST_MULTI_CALC)
	@echo "GoogleTest coverage test executables built successfully"

# Build coverage gtest_test_calc
$(GTEST_COV_TEST_CALC): $(GTEST_COV_UT_OUTPUT_DIR)/test_calc.o $(GTEST_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CXX) $< -o $@ $(GTEST_COV_UT_LDFLAGS)

# Build coverage gtest_test_greeting
$(GTEST_COV_TEST_GREETING): $(GTEST_COV_UT_OUTPUT_DIR)/test_greeting.o $(GTEST_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CXX) $< -o $@ $(GTEST_COV_UT_LDFLAGS)

# Build coverage gtest_test_multi_calc (with mock)
$(GTEST_COV_TEST_MULTI_CALC): $(GTEST_COV_UT_OUTPUT_DIR)/test_multi_calc.o $(GTEST_COV_SDK_LIB)
	@echo "Building coverage test (with mock): $@"
	$(CXX) $< -o $@ $(GTEST_COV_MOCK_LDFLAGS)

# Compile UT source files with coverage (C++ files)
$(GTEST_COV_UT_OUTPUT_DIR)/%.o: $(GTEST_SRC_DIR)/%.cpp
	@echo "Compiling test (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CXX) $(GTEST_COV_UT_CXXFLAGS) -c $< -o $@

# Clean GoogleTest coverage artifacts
.PHONY: clean-gtest-cov
clean-gtest-cov:
	$(RM) $(GTEST_COV_OUTPUT_DIR) $(GTEST_COV_REPORT_DIR)
	@find . -name "*.gcno" -path "*/coverage_gtest/*" -delete 2>/dev/null || true
	@find . -name "*.gcda" -path "*/coverage_gtest/*" -delete 2>/dev/null || true
	@find . -name "*.gcov" -path "*/coverage_gtest/*" -delete 2>/dev/null || true
