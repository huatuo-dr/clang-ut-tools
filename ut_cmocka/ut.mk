# Unit test build rules

# CMocka directories
CMOCKA_DIR := ut_cmocka/cmocka-install
CMOCKA_INC_DIR := $(CMOCKA_DIR)/include
CMOCKA_LIB_DIR := $(CMOCKA_DIR)/lib

# UT source files
UT_SRC_DIR := ut_cmocka/src
UT_SRCS := $(wildcard $(UT_SRC_DIR)/*.c)
UT_OBJS := $(patsubst $(UT_SRC_DIR)/%.c, $(UT_OUTPUT_DIR)/%.o, $(UT_SRCS))

# UT executables (one per test file)
UT_TEST_CALC := $(DIST_DIR)/test_calc
UT_TEST_GREETING := $(DIST_DIR)/test_greeting
UT_TEST_MULTI_CALC := $(DIST_DIR)/test_multi_calc

# UT report directory
UT_REPORT_DIR := $(BUILD_DIR)/ut-report

# UT specific flags (use installed SDK from build directory)
UT_CFLAGS := $(CFLAGS) -I$(SDK_INSTALL_INC_DIR) -I$(CMOCKA_INC_DIR)
UT_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -L$(CMOCKA_LIB_DIR) -lsdk -lcmocka -Wl,-rpath,$(CMOCKA_LIB_DIR)

# Mock test specific LDFLAGS (--wrap options for mocking calc functions)
UT_MOCK_LDFLAGS := $(UT_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run all unit tests
.PHONY: ut
ut: ut_run ut_report
	@echo ""
	@echo "========================================"
	@echo "All Unit Tests Completed!"
	@echo "  Reports: $(UT_REPORT_DIR)/"
	@echo "========================================"

# Run unit tests (output to terminal)
.PHONY: ut_run
ut_run: ut_build
	@echo ""
	@echo "========================================"
	@echo "Running Unit Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running test_calc ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(UT_TEST_CALC)
	@echo ""
	@echo "--- Running test_greeting ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(UT_TEST_GREETING)
	@echo ""
	@echo "--- Running test_multi_calc (Mock Tests) ---"
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH $(UT_TEST_MULTI_CALC)

# Generate XML reports and HTML report
.PHONY: ut_report
ut_report: ut_build
	@echo ""
	@echo "========================================"
	@echo "Generating Test Reports..."
	@echo "========================================"
	@$(MKDIR) $(UT_REPORT_DIR)
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(UT_REPORT_DIR)/test_calc_%g.xml \
		$(UT_TEST_CALC) || true
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(UT_REPORT_DIR)/test_greeting_%g.xml \
		$(UT_TEST_GREETING) || true
	@LD_LIBRARY_PATH=$(CMOCKA_LIB_DIR):$$LD_LIBRARY_PATH \
		CMOCKA_MESSAGE_OUTPUT=XML \
		CMOCKA_XML_FILE=$(UT_REPORT_DIR)/test_multi_calc_%g.xml \
		$(UT_TEST_MULTI_CALC) || true
	@echo "Generating HTML report..."
	@cd $(UT_REPORT_DIR) && junit2html --merge merged.xml *.xml && junit2html merged.xml report.html
	@echo ""
	@echo "Reports generated:"
	@echo "  XML: $(UT_REPORT_DIR)/*.xml"
	@echo "  HTML: $(UT_REPORT_DIR)/report.html"

# Build unit tests only (without running)
.PHONY: ut_build
ut_build: sdk_install $(UT_TEST_CALC) $(UT_TEST_GREETING) $(UT_TEST_MULTI_CALC)
	@echo "Unit test executables built successfully"
	@echo "  - $(UT_TEST_CALC)"
	@echo "  - $(UT_TEST_GREETING)"
	@echo "  - $(UT_TEST_MULTI_CALC) (with mock)"

# Build test_calc executable
$(UT_TEST_CALC): $(UT_OUTPUT_DIR)/test_calc.o
	@echo "Building test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UT_LDFLAGS)

# Build test_greeting executable
$(UT_TEST_GREETING): $(UT_OUTPUT_DIR)/test_greeting.o
	@echo "Building test executable: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UT_LDFLAGS)

# Build test_multi_calc executable (with --wrap for mocking)
$(UT_TEST_MULTI_CALC): $(UT_OUTPUT_DIR)/test_multi_calc.o
	@echo "Building test executable (with mock): $@"
	@$(MKDIR) $(dir $@)
	$(CC) $< -o $@ $(UT_MOCK_LDFLAGS)

# Compile UT source files
$(UT_OUTPUT_DIR)/%.o: $(UT_SRC_DIR)/%.c
	@echo "Compiling: $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(UT_CFLAGS) -c $< -o $@

# Clean UT artifacts
.PHONY: clean-ut
clean-ut:
	$(RM) $(UT_OUTPUT_DIR) $(UT_REPORT_DIR) $(UT_TEST_CALC) $(UT_TEST_GREETING) $(UT_TEST_MULTI_CALC)