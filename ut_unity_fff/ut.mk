# Unity + fff unit test build rules

# Unity and fff directories
UNITY_FFF_DIR := ut_unity_fff
UNITY_DIR := $(UNITY_FFF_DIR)/unity-install
UNITY_INC_DIR := $(UNITY_DIR)/include/unity
UNITY_LIB_DIR := $(UNITY_DIR)/lib
FFF_DIR := $(UNITY_FFF_DIR)/fff-install

# UT source files
UNITY_FFF_SRC_DIR := $(UNITY_FFF_DIR)/src
UNITY_FFF_OUTPUT_DIR := $(OUTPUT_DIR)/ut_unity_fff

# UT executables
UNITY_TEST_CALC := $(DIST_DIR)/unity_test_calc
UNITY_TEST_GREETING := $(DIST_DIR)/unity_test_greeting
UNITY_TEST_MULTI_CALC := $(DIST_DIR)/unity_test_multi_calc

# UT report directory
UNITY_REPORT_DIR := $(BUILD_DIR)/ut-unity-report

# UT specific flags
UNITY_CFLAGS := $(CFLAGS) -I$(SDK_INSTALL_INC_DIR) -I$(UNITY_INC_DIR) -I$(FFF_DIR)
UNITY_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -L$(UNITY_LIB_DIR) -lsdk -lunity

# Mock test specific LDFLAGS (--wrap options for mocking calc functions)
UNITY_MOCK_LDFLAGS := $(UNITY_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run all Unity tests
.PHONY: ut_unity
ut_unity: ut_unity_run ut_unity_report
	@echo ""
	@echo "========================================"
	@echo "All Unity Tests Completed!"
	@echo "  Reports: $(UNITY_REPORT_DIR)/"
	@echo "========================================"

# Run Unity tests (output to terminal)
.PHONY: ut_unity_run
ut_unity_run: ut_unity_build
	@echo ""
	@echo "========================================"
	@echo "Running Unity + fff Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running unity_test_calc ---"
	@$(UNITY_TEST_CALC)
	@echo ""
	@echo "--- Running unity_test_greeting ---"
	@$(UNITY_TEST_GREETING)
	@echo ""
	@echo "--- Running unity_test_multi_calc (fff Mock Tests) ---"
	@$(UNITY_TEST_MULTI_CALC)

# Unity to JUnit converter script
UNITY_TO_JUNIT := $(UNITY_FFF_DIR)/unity_to_junit.py

# Generate Unity test reports (TXT + XML + HTML)
.PHONY: ut_unity_report
ut_unity_report: ut_unity_build
	@echo ""
	@echo "========================================"
	@echo "Generating Unity Test Reports..."
	@echo "========================================"
	@$(MKDIR) $(UNITY_REPORT_DIR)
	@echo "Running tests and capturing output..."
	@$(UNITY_TEST_CALC) > $(UNITY_REPORT_DIR)/test_calc.txt 2>&1 || true
	@$(UNITY_TEST_GREETING) > $(UNITY_REPORT_DIR)/test_greeting.txt 2>&1 || true
	@$(UNITY_TEST_MULTI_CALC) > $(UNITY_REPORT_DIR)/test_multi_calc.txt 2>&1 || true
	@echo "Converting to JUnit XML..."
	@cat $(UNITY_REPORT_DIR)/test_calc.txt $(UNITY_REPORT_DIR)/test_greeting.txt $(UNITY_REPORT_DIR)/test_multi_calc.txt > $(UNITY_REPORT_DIR)/all_tests.txt
	@python3 $(UNITY_TO_JUNIT) $(UNITY_REPORT_DIR)/all_tests.txt -o $(UNITY_REPORT_DIR)/unity_results.xml
	@echo "Generating HTML report..."
	@cd $(UNITY_REPORT_DIR) && junit2html unity_results.xml report.html
	@echo ""
	@echo "Reports generated:"
	@echo "  TXT: $(UNITY_REPORT_DIR)/*.txt"
	@echo "  XML: $(UNITY_REPORT_DIR)/unity_results.xml"
	@echo "  HTML: $(UNITY_REPORT_DIR)/report.html"

# Build Unity tests only (without running)
.PHONY: ut_unity_build
ut_unity_build: sdk_install $(UNITY_TEST_CALC) $(UNITY_TEST_GREETING) $(UNITY_TEST_MULTI_CALC)
	@echo "Unity test executables built successfully"
	@echo "  - $(UNITY_TEST_CALC)"
	@echo "  - $(UNITY_TEST_GREETING)"
	@echo "  - $(UNITY_TEST_MULTI_CALC) (with fff mock)"

# Build unity_test_calc executable
$(UNITY_TEST_CALC): $(UNITY_FFF_OUTPUT_DIR)/test_calc.o
	@echo "Building Unity test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UNITY_LDFLAGS)

# Build unity_test_greeting executable
$(UNITY_TEST_GREETING): $(UNITY_FFF_OUTPUT_DIR)/test_greeting.o
	@echo "Building Unity test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UNITY_LDFLAGS)

# Build unity_test_multi_calc executable (with --wrap for mocking)
$(UNITY_TEST_MULTI_CALC): $(UNITY_FFF_OUTPUT_DIR)/test_multi_calc.o
	@echo "Building Unity test executable (with fff mock): $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UNITY_MOCK_LDFLAGS)

# Compile Unity test source files
$(UNITY_FFF_OUTPUT_DIR)/%.o: $(UNITY_FFF_SRC_DIR)/%.c
	@echo "Compiling Unity test: $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(UNITY_CFLAGS) -c $< -o $@

# Clean Unity UT artifacts
.PHONY: clean-ut-unity
clean-ut-unity:
	$(RM) $(UNITY_FFF_OUTPUT_DIR) $(UNITY_REPORT_DIR) $(UNITY_TEST_CALC) $(UNITY_TEST_GREETING) $(UNITY_TEST_MULTI_CALC)
