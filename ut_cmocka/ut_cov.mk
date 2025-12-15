# Coverage build rules for CMocka unit tests
# Uses gcov + lcov + genhtml to generate HTML coverage reports

# Coverage directories
CMOCKA_COV_OUTPUT_DIR := $(OUTPUT_DIR)/coverage_cmocka
CMOCKA_COV_SDK_OUTPUT_DIR := $(CMOCKA_COV_OUTPUT_DIR)/sdk
CMOCKA_COV_UT_OUTPUT_DIR := $(CMOCKA_COV_OUTPUT_DIR)/ut
CMOCKA_COV_REPORT_DIR := $(BUILD_DIR)/coverage-cmocka-report

# Coverage flags
CMOCKA_COV_CFLAGS := $(CFLAGS) --coverage -fprofile-arcs -ftest-coverage
CMOCKA_COV_LDFLAGS := --coverage

# Coverage SDK object files
CMOCKA_COV_SDK_SRCS := $(wildcard sdk/src/*.c)
CMOCKA_COV_SDK_OBJS := $(patsubst sdk/src/%.c, $(CMOCKA_COV_SDK_OUTPUT_DIR)/%.o, $(CMOCKA_COV_SDK_SRCS))

# Coverage UT object files
CMOCKA_COV_UT_SRCS := $(wildcard $(CMOCKA_SRC_DIR)/*.c)
CMOCKA_COV_UT_OBJS := $(patsubst $(CMOCKA_SRC_DIR)/%.c, $(CMOCKA_COV_UT_OUTPUT_DIR)/%.o, $(CMOCKA_COV_UT_SRCS))

# Coverage test executables
CMOCKA_COV_TEST_CALC := $(CMOCKA_COV_OUTPUT_DIR)/cmocka_test_calc
CMOCKA_COV_TEST_GREETING := $(CMOCKA_COV_OUTPUT_DIR)/cmocka_test_greeting
CMOCKA_COV_TEST_MULTI_CALC := $(CMOCKA_COV_OUTPUT_DIR)/cmocka_test_multi_calc

# Coverage SDK library
CMOCKA_COV_SDK_LIB := $(CMOCKA_COV_OUTPUT_DIR)/libsdk_cov.a

# UT specific flags for coverage build
CMOCKA_COV_UT_CFLAGS := $(CMOCKA_COV_CFLAGS) -Isdk/include -I$(CMOCKA_INC_DIR)
CMOCKA_COV_UT_LDFLAGS := $(CMOCKA_COV_LDFLAGS) -L$(CMOCKA_COV_OUTPUT_DIR) -L$(CMOCKA_LIB_DIR) -lsdk_cov -lcmocka -Wl,-rpath,$(CMOCKA_LIB_DIR)

# Mock test specific LDFLAGS for coverage
CMOCKA_COV_MOCK_LDFLAGS := $(CMOCKA_COV_UT_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run coverage tests, then generate report
.PHONY: ut_cmocka_cov
ut_cmocka_cov: ut_cmocka_cov_run ut_cmocka_cov_report
	@echo ""
	@echo "========================================"
	@echo "CMocka Coverage Report Generated!"
	@echo "  HTML: $(CMOCKA_COV_REPORT_DIR)/index.html"
	@echo "========================================"

# Run coverage tests
.PHONY: ut_cmocka_cov_run
ut_cmocka_cov_run: ut_cmocka_cov_build
	@echo ""
	@echo "========================================"
	@echo "Running CMocka Coverage Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running cmocka_test_calc (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_COV_TEST_CALC)
	@echo ""
	@echo "--- Running cmocka_test_greeting (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_COV_TEST_GREETING)
	@echo ""
	@echo "--- Running cmocka_test_multi_calc (coverage) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_COV_TEST_MULTI_CALC)

# Generate coverage report using lcov and genhtml
.PHONY: ut_cmocka_cov_report
ut_cmocka_cov_report: ut_cmocka_cov_run
	@echo ""
	@echo "========================================"
	@echo "Generating CMocka Coverage Report..."
	@echo "========================================"
	@$(MKDIR) $(CMOCKA_COV_REPORT_DIR)
	@echo "Collecting coverage data..."
	@lcov --capture --directory $(CMOCKA_COV_OUTPUT_DIR) --output-file $(CMOCKA_COV_REPORT_DIR)/coverage.info
	@echo "Filtering out system headers..."
	@lcov --remove $(CMOCKA_COV_REPORT_DIR)/coverage.info '/usr/*' --output-file $(CMOCKA_COV_REPORT_DIR)/coverage_filtered.info
	@echo "Generating HTML report..."
	@genhtml $(CMOCKA_COV_REPORT_DIR)/coverage_filtered.info --output-directory $(CMOCKA_COV_REPORT_DIR) --title "CMocka SDK Coverage Report" --legend
	@echo ""
	@echo "Coverage report generated:"
	@echo "  $(CMOCKA_COV_REPORT_DIR)/index.html"

# Build coverage SDK library
$(CMOCKA_COV_SDK_LIB): $(CMOCKA_COV_SDK_OBJS)
	@echo "Building coverage SDK library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Compile SDK source files with coverage
$(CMOCKA_COV_SDK_OUTPUT_DIR)/%.o: sdk/src/%.c
	@echo "Compiling (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(CMOCKA_COV_CFLAGS) -Isdk/include -c $< -o $@

# Build coverage test executables
.PHONY: ut_cmocka_cov_build
ut_cmocka_cov_build: $(CMOCKA_COV_SDK_LIB) $(CMOCKA_COV_TEST_CALC) $(CMOCKA_COV_TEST_GREETING) $(CMOCKA_COV_TEST_MULTI_CALC)
	@echo "CMocka coverage test executables built successfully"

# Build coverage cmocka_test_calc
$(CMOCKA_COV_TEST_CALC): $(CMOCKA_COV_UT_OUTPUT_DIR)/test_calc.o $(CMOCKA_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(CMOCKA_COV_UT_LDFLAGS)

# Build coverage cmocka_test_greeting
$(CMOCKA_COV_TEST_GREETING): $(CMOCKA_COV_UT_OUTPUT_DIR)/test_greeting.o $(CMOCKA_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(CMOCKA_COV_UT_LDFLAGS)

# Build coverage cmocka_test_multi_calc (with mock)
$(CMOCKA_COV_TEST_MULTI_CALC): $(CMOCKA_COV_UT_OUTPUT_DIR)/test_multi_calc.o $(CMOCKA_COV_SDK_LIB)
	@echo "Building coverage test (with mock): $@"
	$(CC) $< -o $@ $(CMOCKA_COV_MOCK_LDFLAGS)

# Compile UT source files with coverage
$(CMOCKA_COV_UT_OUTPUT_DIR)/%.o: $(CMOCKA_SRC_DIR)/%.c
	@echo "Compiling test (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(CMOCKA_COV_UT_CFLAGS) -c $< -o $@

# Clean CMocka coverage artifacts
.PHONY: clean-cmocka-cov
clean-cmocka-cov:
	$(RM) $(CMOCKA_COV_OUTPUT_DIR) $(CMOCKA_COV_REPORT_DIR)
	@find . -name "*.gcno" -path "*/coverage_cmocka/*" -delete 2>/dev/null || true
	@find . -name "*.gcda" -path "*/coverage_cmocka/*" -delete 2>/dev/null || true
	@find . -name "*.gcov" -path "*/coverage_cmocka/*" -delete 2>/dev/null || true
