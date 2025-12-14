# GoogleTest + mockcpp unit test build rules

# Directories
GTEST_MOCKCPP_DIR := ut_gtest_mockcpp
GTEST_MOCKCPP_GTEST_DIR := $(GTEST_MOCKCPP_DIR)/gtest-install
GTEST_MOCKCPP_MOCKCPP_DIR := $(GTEST_MOCKCPP_DIR)/mockcpp-install

# Include and lib paths
GTEST_MOCKCPP_GTEST_INC := $(GTEST_MOCKCPP_GTEST_DIR)/include
GTEST_MOCKCPP_GTEST_LIB := $(GTEST_MOCKCPP_GTEST_DIR)/lib
GTEST_MOCKCPP_MOCKCPP_INC := $(GTEST_MOCKCPP_MOCKCPP_DIR)/include
GTEST_MOCKCPP_MOCKCPP_LIB := $(GTEST_MOCKCPP_MOCKCPP_DIR)/lib

# Source and output directories
GTEST_MOCKCPP_SRC_DIR := $(GTEST_MOCKCPP_DIR)/src
GTEST_MOCKCPP_OUTPUT_DIR := $(OUTPUT_DIR)/ut_gtest_mockcpp

# Test executables
GTEST_MOCKCPP_TEST_MULTI_CALC := $(DIST_DIR)/gtest_mockcpp_test_multi_calc

# Report directory
GTEST_MOCKCPP_REPORT_DIR := $(BUILD_DIR)/ut-gtest-mockcpp-report

# C++ compiler flags
GTEST_MOCKCPP_CXXFLAGS := $(CXXFLAGS) -I$(SDK_INSTALL_INC_DIR) -I$(GTEST_MOCKCPP_GTEST_INC) -I$(GTEST_MOCKCPP_MOCKCPP_INC)

# Linker flags (no --wrap needed, mockcpp uses runtime API hooking)
GTEST_MOCKCPP_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -L$(GTEST_MOCKCPP_GTEST_LIB) -L$(GTEST_MOCKCPP_MOCKCPP_LIB) \
    -lsdk -lgtest -lmockcpp -lpthread

# Build and run all tests
.PHONY: ut_gtest_mockcpp
ut_gtest_mockcpp: ut_gtest_mockcpp_run ut_gtest_mockcpp_report
	@echo ""
	@echo "========================================"
	@echo "All GoogleTest + mockcpp Tests Completed!"
	@echo "  Reports: $(GTEST_MOCKCPP_REPORT_DIR)/"
	@echo "========================================"

# Run tests (output to terminal)
.PHONY: ut_gtest_mockcpp_run
ut_gtest_mockcpp_run: ut_gtest_mockcpp_build
	@echo ""
	@echo "========================================"
	@echo "Running GoogleTest + mockcpp Tests..."
	@echo "========================================"
	@echo ""
	@echo "--- Running gtest_mockcpp_test_multi_calc ---"
	@$(GTEST_MOCKCPP_TEST_MULTI_CALC)

# Generate test reports (XML + HTML)
.PHONY: ut_gtest_mockcpp_report
ut_gtest_mockcpp_report: ut_gtest_mockcpp_build
	@echo ""
	@echo "========================================"
	@echo "Generating GoogleTest + mockcpp Reports..."
	@echo "========================================"
	@$(MKDIR) $(GTEST_MOCKCPP_REPORT_DIR)
	@echo "Running tests with XML output..."
	@$(GTEST_MOCKCPP_TEST_MULTI_CALC) --gtest_output=xml:$(GTEST_MOCKCPP_REPORT_DIR)/test_multi_calc.xml || true
	@echo "Generating HTML report..."
	@cd $(GTEST_MOCKCPP_REPORT_DIR) && junit2html test_multi_calc.xml report.html
	@echo ""
	@echo "Reports generated:"
	@echo "  XML: $(GTEST_MOCKCPP_REPORT_DIR)/*.xml"
	@echo "  HTML: $(GTEST_MOCKCPP_REPORT_DIR)/report.html"

# Build tests only (without running)
.PHONY: ut_gtest_mockcpp_build
ut_gtest_mockcpp_build: sdk_install $(GTEST_MOCKCPP_TEST_MULTI_CALC)
	@echo "GoogleTest + mockcpp executables built successfully"
	@echo "  - $(GTEST_MOCKCPP_TEST_MULTI_CALC)"

# Build test executable
$(GTEST_MOCKCPP_TEST_MULTI_CALC): $(GTEST_MOCKCPP_OUTPUT_DIR)/test_multi_calc.o
	@echo "Building GoogleTest + mockcpp executable: $@"
	@$(MKDIR) $(dir $@)
	$(CXX) $< -o $@ $(GTEST_MOCKCPP_LDFLAGS)

# Compile test source files
$(GTEST_MOCKCPP_OUTPUT_DIR)/%.o: $(GTEST_MOCKCPP_SRC_DIR)/%.cpp
	@echo "Compiling GoogleTest + mockcpp test: $<"
	@$(MKDIR) $(dir $@)
	$(CXX) $(GTEST_MOCKCPP_CXXFLAGS) -c $< -o $@

# Clean artifacts
.PHONY: clean-ut-gtest-mockcpp
clean-ut-gtest-mockcpp:
	$(RM) $(GTEST_MOCKCPP_OUTPUT_DIR) $(GTEST_MOCKCPP_REPORT_DIR) $(GTEST_MOCKCPP_TEST_MULTI_CALC)
