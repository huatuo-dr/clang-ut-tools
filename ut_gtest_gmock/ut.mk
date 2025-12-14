# GoogleTest + GMock unit test build rules

# GoogleTest directories
GTEST_DIR := ut_gtest_gmock
GTEST_INSTALL_DIR := $(GTEST_DIR)/gtest-install
GTEST_INC_DIR := $(GTEST_INSTALL_DIR)/include
GTEST_LIB_DIR := $(GTEST_INSTALL_DIR)/lib

# UT source files
GTEST_SRC_DIR := $(GTEST_DIR)/src
GTEST_OUTPUT_DIR := $(OUTPUT_DIR)/ut_gtest

# UT executables
GTEST_TEST_CALC := $(DIST_DIR)/gtest_test_calc
GTEST_TEST_GREETING := $(DIST_DIR)/gtest_test_greeting
GTEST_TEST_MULTI_CALC := $(DIST_DIR)/gtest_test_multi_calc

# UT report directory
GTEST_REPORT_DIR := $(BUILD_DIR)/ut-gtest-report

# C++ compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -g -std=c++17

# UT specific flags
GTEST_CXXFLAGS := $(CXXFLAGS) -I$(SDK_INSTALL_INC_DIR) -I$(GTEST_INC_DIR)
GTEST_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -L$(GTEST_LIB_DIR) -lsdk -lgtest -lgmock -lpthread

# Mock test specific LDFLAGS (--wrap options for mocking calc functions)
GTEST_MOCK_LDFLAGS := $(GTEST_LDFLAGS) \
    -Wl,--wrap=calc_add \
    -Wl,--wrap=calc_subtract \
    -Wl,--wrap=calc_multiply \
    -Wl,--wrap=calc_divide

# Build and run all GoogleTest tests
.PHONY: ut_gtest
ut_gtest: ut_gtest_run ut_gtest_report
	@echo ""
	@echo "========================================"
	@echo "All GoogleTest Tests Completed!"
	@echo "  Reports: $(GTEST_REPORT_DIR)/"
	@echo "========================================"

# Run GoogleTest tests (output to terminal)
.PHONY: ut_gtest_run
ut_gtest_run: ut_gtest_build
	@echo ""
	@echo "========================================"
	@echo "Running GoogleTest + GMock Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running gtest_test_calc ---"
	@$(GTEST_TEST_CALC)
	@echo ""
	@echo "--- Running gtest_test_greeting ---"
	@$(GTEST_TEST_GREETING)
	@echo ""
	@echo "--- Running gtest_test_multi_calc (GMock Tests) ---"
	@$(GTEST_TEST_MULTI_CALC)

# Generate GoogleTest reports (XML + HTML)
.PHONY: ut_gtest_report
ut_gtest_report: ut_gtest_build
	@echo ""
	@echo "========================================"
	@echo "Generating GoogleTest Reports..."
	@echo "========================================"
	@$(MKDIR) $(GTEST_REPORT_DIR)
	@echo "Running tests with XML output..."
	@$(GTEST_TEST_CALC) --gtest_output=xml:$(GTEST_REPORT_DIR)/test_calc.xml || true
	@$(GTEST_TEST_GREETING) --gtest_output=xml:$(GTEST_REPORT_DIR)/test_greeting.xml || true
	@$(GTEST_TEST_MULTI_CALC) --gtest_output=xml:$(GTEST_REPORT_DIR)/test_multi_calc.xml || true
	@echo "Merging XML reports..."
	@cd $(GTEST_REPORT_DIR) && junit2html --merge merged.xml *.xml
	@echo "Generating HTML report..."
	@cd $(GTEST_REPORT_DIR) && junit2html merged.xml report.html
	@echo ""
	@echo "Reports generated:"
	@echo "  XML: $(GTEST_REPORT_DIR)/*.xml"
	@echo "  HTML: $(GTEST_REPORT_DIR)/report.html"

# Build GoogleTest tests only (without running)
.PHONY: ut_gtest_build
ut_gtest_build: sdk_install $(GTEST_TEST_CALC) $(GTEST_TEST_GREETING) $(GTEST_TEST_MULTI_CALC)
	@echo "GoogleTest executables built successfully"
	@echo "  - $(GTEST_TEST_CALC)"
	@echo "  - $(GTEST_TEST_GREETING)"
	@echo "  - $(GTEST_TEST_MULTI_CALC) (with GMock)"

# Build gtest_test_calc executable
$(GTEST_TEST_CALC): $(GTEST_OUTPUT_DIR)/test_calc.o
	@echo "Building GoogleTest executable: $@"
	@$(MKDIR) $(dir $@)
	$(CXX) $< -o $@ $(GTEST_LDFLAGS)

# Build gtest_test_greeting executable
$(GTEST_TEST_GREETING): $(GTEST_OUTPUT_DIR)/test_greeting.o
	@echo "Building GoogleTest executable: $@"
	@$(MKDIR) $(dir $@)
	$(CXX) $< -o $@ $(GTEST_LDFLAGS)

# Build gtest_test_multi_calc executable (with --wrap for mocking)
$(GTEST_TEST_MULTI_CALC): $(GTEST_OUTPUT_DIR)/test_multi_calc.o
	@echo "Building GoogleTest executable (with GMock): $@"
	@$(MKDIR) $(dir $@)
	$(CXX) $< -o $@ $(GTEST_MOCK_LDFLAGS)

# Compile GoogleTest source files
$(GTEST_OUTPUT_DIR)/%.o: $(GTEST_SRC_DIR)/%.cpp
	@echo "Compiling GoogleTest: $<"
	@$(MKDIR) $(dir $@)
	$(CXX) $(GTEST_CXXFLAGS) -c $< -o $@

# Clean GoogleTest artifacts
.PHONY: clean-ut-gtest
clean-ut-gtest:
	$(RM) $(GTEST_OUTPUT_DIR) $(GTEST_REPORT_DIR) $(GTEST_TEST_CALC) $(GTEST_TEST_GREETING) $(GTEST_TEST_MULTI_CALC)
