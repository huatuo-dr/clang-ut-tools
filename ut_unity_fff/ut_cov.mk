# Coverage build rules for Unity + fff unit tests
# Uses gcov + lcov + genhtml to generate HTML coverage reports

# Coverage directories
UNITY_COV_OUTPUT_DIR := $(OUTPUT_DIR)/coverage_unity
UNITY_COV_SDK_OUTPUT_DIR := $(UNITY_COV_OUTPUT_DIR)/sdk
UNITY_COV_UT_OUTPUT_DIR := $(UNITY_COV_OUTPUT_DIR)/ut
UNITY_COV_REPORT_DIR := $(BUILD_DIR)/coverage-unity-report

# Coverage flags
UNITY_COV_CFLAGS := $(CFLAGS) --coverage -fprofile-arcs -ftest-coverage
UNITY_COV_LDFLAGS := --coverage

# Coverage SDK object files
UNITY_COV_SDK_SRCS := $(wildcard sdk/src/*.c)
UNITY_COV_SDK_OBJS := $(patsubst sdk/src/%.c, $(UNITY_COV_SDK_OUTPUT_DIR)/%.o, $(UNITY_COV_SDK_SRCS))

# Coverage UT object files
UNITY_COV_UT_SRCS := $(wildcard $(UNITY_FFF_SRC_DIR)/*.c)
UNITY_COV_UT_OBJS := $(patsubst $(UNITY_FFF_SRC_DIR)/%.c, $(UNITY_COV_UT_OUTPUT_DIR)/%.o, $(UNITY_COV_UT_SRCS))

# Coverage test executables
UNITY_COV_TEST_CALC := $(UNITY_COV_OUTPUT_DIR)/unity_test_calc
UNITY_COV_TEST_GREETING := $(UNITY_COV_OUTPUT_DIR)/unity_test_greeting
UNITY_COV_TEST_MULTI_CALC := $(UNITY_COV_OUTPUT_DIR)/unity_test_multi_calc

# Coverage SDK library
UNITY_COV_SDK_LIB := $(UNITY_COV_OUTPUT_DIR)/libsdk_cov.a

# UT specific flags for coverage build
UNITY_COV_UT_CFLAGS := $(UNITY_COV_CFLAGS) -Isdk/include -I$(UNITY_INC_DIR) -I$(FFF_DIR)
UNITY_COV_UT_LDFLAGS := $(UNITY_COV_LDFLAGS) -L$(UNITY_COV_OUTPUT_DIR) -L$(UNITY_LIB_DIR) -lsdk_cov -lunity

# Mock test specific LDFLAGS for coverage
UNITY_COV_MOCK_LDFLAGS := $(UNITY_COV_UT_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run coverage tests, then generate report
.PHONY: ut_unity_cov
ut_unity_cov: ut_unity_cov_run ut_unity_cov_report
	@echo ""
	@echo "========================================"
	@echo "Unity Coverage Report Generated!"
	@echo "  HTML: $(UNITY_COV_REPORT_DIR)/index.html"
	@echo "========================================"

# Run coverage tests
.PHONY: ut_unity_cov_run
ut_unity_cov_run: ut_unity_cov_build
	@echo ""
	@echo "========================================"
	@echo "Running Unity Coverage Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running unity_test_calc (coverage) ---"
	@$(UNITY_COV_TEST_CALC)
	@echo ""
	@echo "--- Running unity_test_greeting (coverage) ---"
	@$(UNITY_COV_TEST_GREETING)
	@echo ""
	@echo "--- Running unity_test_multi_calc (coverage) ---"
	@$(UNITY_COV_TEST_MULTI_CALC)

# Generate coverage report using lcov and genhtml
.PHONY: ut_unity_cov_report
ut_unity_cov_report: ut_unity_cov_run
	@echo ""
	@echo "========================================"
	@echo "Generating Unity Coverage Report..."
	@echo "========================================"
	@$(MKDIR) $(UNITY_COV_REPORT_DIR)
	@echo "Collecting coverage data..."
	@lcov --capture --directory $(UNITY_COV_OUTPUT_DIR) --output-file $(UNITY_COV_REPORT_DIR)/coverage.info
	@echo "Filtering out system headers..."
	@lcov --remove $(UNITY_COV_REPORT_DIR)/coverage.info '/usr/*' --output-file $(UNITY_COV_REPORT_DIR)/coverage_filtered.info
	@echo "Generating HTML report..."
	@genhtml $(UNITY_COV_REPORT_DIR)/coverage_filtered.info --output-directory $(UNITY_COV_REPORT_DIR) --title "Unity SDK Coverage Report" --legend
	@echo ""
	@echo "Coverage report generated:"
	@echo "  $(UNITY_COV_REPORT_DIR)/index.html"

# Build coverage SDK library
$(UNITY_COV_SDK_LIB): $(UNITY_COV_SDK_OBJS)
	@echo "Building coverage SDK library: $@"
	$(AR) $(ARFLAGS) $@ $^

# Compile SDK source files with coverage
$(UNITY_COV_SDK_OUTPUT_DIR)/%.o: sdk/src/%.c
	@echo "Compiling (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(UNITY_COV_CFLAGS) -Isdk/include -c $< -o $@

# Build coverage test executables
.PHONY: ut_unity_cov_build
ut_unity_cov_build: $(UNITY_COV_SDK_LIB) $(UNITY_COV_TEST_CALC) $(UNITY_COV_TEST_GREETING) $(UNITY_COV_TEST_MULTI_CALC)
	@echo "Unity coverage test executables built successfully"

# Build coverage unity_test_calc
$(UNITY_COV_TEST_CALC): $(UNITY_COV_UT_OUTPUT_DIR)/test_calc.o $(UNITY_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(UNITY_COV_UT_LDFLAGS)

# Build coverage unity_test_greeting
$(UNITY_COV_TEST_GREETING): $(UNITY_COV_UT_OUTPUT_DIR)/test_greeting.o $(UNITY_COV_SDK_LIB)
	@echo "Building coverage test: $@"
	$(CC) $< -o $@ $(UNITY_COV_UT_LDFLAGS)

# Build coverage unity_test_multi_calc (with mock)
$(UNITY_COV_TEST_MULTI_CALC): $(UNITY_COV_UT_OUTPUT_DIR)/test_multi_calc.o $(UNITY_COV_SDK_LIB)
	@echo "Building coverage test (with mock): $@"
	$(CC) $< -o $@ $(UNITY_COV_MOCK_LDFLAGS)

# Compile UT source files with coverage
$(UNITY_COV_UT_OUTPUT_DIR)/%.o: $(UNITY_FFF_SRC_DIR)/%.c
	@echo "Compiling test (coverage): $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(UNITY_COV_UT_CFLAGS) -c $< -o $@

# Clean Unity coverage artifacts
.PHONY: clean-unity-cov
clean-unity-cov:
	$(RM) $(UNITY_COV_OUTPUT_DIR) $(UNITY_COV_REPORT_DIR)
	@find . -name "*.gcno" -path "*/coverage_unity/*" -delete 2>/dev/null || true
	@find . -name "*.gcda" -path "*/coverage_unity/*" -delete 2>/dev/null || true
	@find . -name "*.gcov" -path "*/coverage_unity/*" -delete 2>/dev/null || true
