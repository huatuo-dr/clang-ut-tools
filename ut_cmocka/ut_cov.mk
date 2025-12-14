# Coverage build rules for cmocka unit tests
# Uses gcov + lcov + genhtml to generate HTML coverage reports

# Coverage directories
COV_OUTPUT_DIR := $(OUTPUT_DIR)/coverage_cmocka
COV_SDK_OUTPUT_DIR := $(COV_OUTPUT_DIR)/sdk
COV_UT_OUTPUT_DIR := $(COV_OUTPUT_DIR)/ut
COV_REPORT_DIR := $(BUILD_DIR)/coverage-cmocka-report

# Coverage flags
COV_CFLAGS := $(CFLAGS) --coverage -fprofile-arcs -ftest-coverage
COV_LDFLAGS := --coverage

# Coverage SDK object files
COV_SDK_SRCS := $(wildcard sdk/src/*.c)
COV_SDK_OBJS := $(patsubst sdk/src/%.c, $(COV_SDK_OUTPUT_DIR)/%.o, $(COV_SDK_SRCS))

# Coverage UT object files
COV_UT_SRCS := $(wildcard $(UT_SRC_DIR)/*.c)
COV_UT_OBJS := $(patsubst $(UT_SRC_DIR)/%.c, $(COV_UT_OUTPUT_DIR)/%.o, $(COV_UT_SRCS))

# Coverage test executables
COV_TEST_CALC := $(COV_OUTPUT_DIR)/test_calc
COV_TEST_GREETING := $(COV_OUTPUT_DIR)/test_greeting
COV_TEST_MULTI_CALC := $(COV_OUTPUT_DIR)/test_multi_calc

# Coverage SDK library
COV_SDK_LIB := $(COV_OUTPUT_DIR)/libsdk_cov.a

# UT specific flags for coverage build
COV_UT_CFLAGS := $(COV_CFLAGS) -Isdk/include -I$(CMOCKA_INC_DIR)
COV_UT_LDFLAGS := $(COV_LDFLAGS) -L$(COV_OUTPUT_DIR) -L$(CMOCKA_LIB_DIR) -lsdk_cov -lcmocka -Wl,-rpath,$(CMOCKA_LIB_DIR)

# Mock test specific LDFLAGS for coverage
COV_MOCK_LDFLAGS := $(COV_UT_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run coverage tests, then generate report
.PHONY: ut_cov
ut_cov: ut_cov_run ut_cov_report
	@echo ""
	@echo "========================================"
	@echo "Coverage Report Generated!"
	@echo "  HTML: $(COV_REPORT_DIR)/index.html"
	@echo "========================================"

# Run coverage tests
.PHONY: ut_cov_run
ut_cov_run: ut_cov_build
	@echo ""
	@echo "========================================"
	@echo "Running Coverage Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running test_calc (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(COV_TEST_CALC)
	@echo ""
	@echo "--- Running test_greeting (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(COV_TEST_GREETING)
	@echo ""
	@echo "--- Running test_multi_calc (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(COV_TEST_MULTI_CALC)

# Generate coverage report using lcov and genhtml
.PHONY: ut_cov_report
ut_cov_report: ut_cov_run
	@echo ""
	@echo "========================================"
	@echo "Generating Coverage Report..."
	@echo "========================================"
	@$(MKDIR) $(COV_REPORT_DIR)
	@echo "Collecting coverage data..."
	@lcov --capture --directory $(COV_OUTPUT_DIR) --output-file $(COV_REPORT_DIR)/coverage.info
	@echo "Filtering out system headers..."
	@lcov --remove $(COV_REPORT_DIR)/coverage.info '/usr/*' --output-file $(COV_REPORT_DIR)/coverage_filtered.info
	@echo "Generating HTML report..."
	@genhtml $(COV_REPORT_DIR)/coverage_filtered.info --output-directory $(COV_REPORT_DIR) --title "SDK Coverage Report" --legend
	@echo ""
	@echo "Coverage report generated:"
	@echo "  $(COV_REPORT_DIR)/index.html"

# Build coverage SDK library
$(COV_SDK_LIB): $(COV_SDK_OBJS)
	@echo "Building coverage SDK library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Compile SDK source files with coverage
$(COV_SDK_OUTPUT_DIR)/%.o: sdk/src/%.c
	@echo "Compiling (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(COV_CFLAGS) -Isdk/include -c $< -o $@

# Build coverage test executables
.PHONY: ut_cov_build
ut_cov_build: $(COV_SDK_LIB) $(COV_TEST_CALC) $(COV_TEST_GREETING) $(COV_TEST_MULTI_CALC)
	@echo "Coverage test executables built successfully"

# Build coverage test_calc
$(COV_TEST_CALC): $(COV_UT_OUTPUT_DIR)/test_calc.o $(COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(COV_UT_LDFLAGS)

# Build coverage test_greeting
$(COV_TEST_GREETING): $(COV_UT_OUTPUT_DIR)/test_greeting.o $(COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(COV_UT_LDFLAGS)

# Build coverage test_multi_calc (with mock)
$(COV_TEST_MULTI_CALC): $(COV_UT_OUTPUT_DIR)/test_multi_calc.o $(COV_SDK_LIB)
	@echo "Building coverage test (with mock): $@"
	$(CC) $< -o $@ $(COV_MOCK_LDFLAGS)

# Compile UT source files with coverage
$(COV_UT_OUTPUT_DIR)/%.o: $(UT_SRC_DIR)/%.c
	@echo "Compiling test (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(COV_UT_CFLAGS) -c $< -o $@

# Clean coverage artifacts
.PHONY: clean-cov
clean-cov:
	$(RM) $(COV_OUTPUT_DIR) $(COV_REPORT_DIR)
	@find . -name "*.gcno" -delete 2>/dev/null || true
	@find . -name "*.gcda" -delete 2>/dev/null || true
	@find . -name "*.gcov" -delete 2>/dev/null || true
