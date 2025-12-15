# CMocka unit test build rules

# CMocka directories
CMOCKA_DIR := ut_cmocka/cmocka-install
CMOCKA_INC_DIR := $(CMOCKA_DIR)/include
CMOCKA_LIB_DIR := $(CMOCKA_DIR)/lib

# UT source files
CMOCKA_SRC_DIR := ut_cmocka/src
CMOCKA_SRCS := $(wildcard $(CMOCKA_SRC_DIR)/*.c)
CMOCKA_OBJS := $(patsubst $(CMOCKA_SRC_DIR)/%.c, $(UT_OUTPUT_DIR)/%.o, $(CMOCKA_SRCS))

# UT executables (one per test file)
CMOCKA_TEST_CALC := $(DIST_DIR)/cmocka_test_calc
CMOCKA_TEST_GREETING := $(DIST_DIR)/cmocka_test_greeting
CMOCKA_TEST_MULTI_CALC := $(DIST_DIR)/cmocka_test_multi_calc

# UT report directory
CMOCKA_REPORT_DIR := $(BUILD_DIR)/ut-cmocka-report

# UT specific flags (use installed SDK from build directory)
CMOCKA_CFLAGS := $(CFLAGS) -I$(SDK_INSTALL_INC_DIR) -I$(CMOCKA_INC_DIR)
CMOCKA_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -L$(CMOCKA_LIB_DIR) -lsdk -lcmocka -Wl,-rpath,$(CMOCKA_LIB_DIR)

# Mock test specific LDFLAGS (--wrap options for mocking calc functions)
CMOCKA_MOCK_LDFLAGS := $(CMOCKA_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run all unit tests
.PHONY: ut_cmocka
ut_cmocka: ut_cmocka_run ut_cmocka_report
	@echo ""
	@echo "========================================"
	@echo "All CMocka Unit Tests Completed!"
	@echo "  Reports: $(CMOCKA_REPORT_DIR)/"
	@echo "========================================"

# Run unit tests (output to terminal)
.PHONY: ut_cmocka_run
ut_cmocka_run: ut_cmocka_build
	@echo ""
	@echo "========================================"
	@echo "Running CMocka Unit Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running cmocka_test_calc ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_TEST_CALC)
	@echo ""
	@echo "--- Running cmocka_test_greeting ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_TEST_GREETING)
	@echo ""
	@echo "--- Running cmocka_test_multi_calc (Mock Tests) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(CMOCKA_TEST_MULTI_CALC)

# Generate XML reports and HTML report
.PHONY: ut_cmocka_report
ut_cmocka_report: ut_cmocka_build
	@echo ""
	@echo "========================================"
	@echo "Generating CMocka Test Reports..."
	@echo "========================================"
	@$(MKDIR) $(CMOCKA_REPORT_DIR)
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(CMOCKA_REPORT_DIR)/test_calc_%g.xml \
		$(CMOCKA_TEST_CALC) || true
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(CMOCKA_REPORT_DIR)/test_greeting_%g.xml \
		$(CMOCKA_TEST_GREETING) || true
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(CMOCKA_REPORT_DIR)/test_multi_calc_%g.xml \
		$(CMOCKA_TEST_MULTI_CALC) || true
	@echo "Generating HTML report..."
	@cd $(CMOCKA_REPORT_DIR) && junit2html --merge merged.xml *.xml && junit2html merged.xml report.html
	@echo ""
	@echo "Reports generated:"
	@echo "  XML: $(CMOCKA_REPORT_DIR)/*.xml"
	@echo "  HTML: $(CMOCKA_REPORT_DIR)/report.html"

# Build unit tests only (without running)
.PHONY: ut_cmocka_build
ut_cmocka_build: sdk_install $(CMOCKA_TEST_CALC) $(CMOCKA_TEST_GREETING) $(CMOCKA_TEST_MULTI_CALC)
	@echo "CMocka test executables built successfully"
	@echo "  - $(CMOCKA_TEST_CALC)"
	@echo "  - $(CMOCKA_TEST_GREETING)"
	@echo "  - $(CMOCKA_TEST_MULTI_CALC) (with mock)"

# Build cmocka_test_calc executable
$(CMOCKA_TEST_CALC): $(UT_OUTPUT_DIR)/test_calc.o
	@echo "Building test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(CMOCKA_LDFLAGS)

# Build cmocka_test_greeting executable
$(CMOCKA_TEST_GREETING): $(UT_OUTPUT_DIR)/test_greeting.o
	@echo "Building test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(CMOCKA_LDFLAGS)

# Build cmocka_test_multi_calc executable (with --wrap for mocking)
$(CMOCKA_TEST_MULTI_CALC): $(UT_OUTPUT_DIR)/test_multi_calc.o
	@echo "Building test executable (with mock): $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(CMOCKA_MOCK_LDFLAGS)

# Compile UT source files
$(UT_OUTPUT_DIR)/%.o: $(CMOCKA_SRC_DIR)/%.c
	@echo "Compiling: $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(CMOCKA_CFLAGS) -c $< -o $@

# Clean CMocka UT artifacts
.PHONY: clean-ut-cmocka
clean-ut-cmocka:
	$(RM) $(UT_OUTPUT_DIR) $(CMOCKA_REPORT_DIR) $(CMOCKA_TEST_CALC) $(CMOCKA_TEST_GREETING) $(CMOCKA_TEST_MULTI_CALC)
