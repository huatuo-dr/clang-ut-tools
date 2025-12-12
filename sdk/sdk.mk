# SDK build rules

# SDK source files and objects
SDK_SRC_DIR := sdk/src
SDK_INC_DIR := sdk/include
SDK_SRCS := $(wildcard $(SDK_SRC_DIR)/*.c)
SDK_OBJS := $(patsubst $(SDK_SRC_DIR)/%.c, $(SDK_OUTPUT_DIR)/%.o, $(SDK_SRCS))
SDK_HEADERS := $(wildcard $(SDK_INC_DIR)/*.h)

# SDK library name
SDK_LIB := $(OUTPUT_DIR)/libsdk.a

# SDK specific flags
SDK_CFLAGS := $(CFLAGS) -I$(SDK_INC_DIR)

# Build SDK library
.PHONY: sdk
sdk: $(SDK_LIB)

$(SDK_LIB): $(SDK_OBJS)
	@echo "Building SDK library: $@"
	@$(MKDIR) $(dir $@)
	$(AR) $(ARFLAGS) $@ $^
	@echo "SDK library built successfully: $@"

# Compile SDK source files
$(SDK_OUTPUT_DIR)/%.o: $(SDK_SRC_DIR)/%.c
	@echo "Compiling: $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(SDK_CFLAGS) -c $< -o $@

# Clean SDK artifacts
.PHONY: clean-sdk
clean-sdk:
	$(RM) $(SDK_OUTPUT_DIR) $(SDK_LIB)

# Install SDK to build directory
.PHONY: sdk_install
sdk_install: sdk
	@echo "Installing SDK to $(SDK_INSTALL_DIR)"
	@$(MKDIR) $(SDK_INSTALL_INC_DIR)
	@$(MKDIR) $(SDK_INSTALL_LIB_DIR)
	@cp -v $(SDK_HEADERS) $(SDK_INSTALL_INC_DIR)/
	@cp -v $(SDK_LIB) $(SDK_INSTALL_LIB_DIR)/libsdk.a
	@echo "SDK installed successfully to $(SDK_INSTALL_DIR)"
	@echo "  Headers: $(SDK_INSTALL_INC_DIR)"
	@echo "  Library: $(SDK_INSTALL_LIB_DIR)/libsdk.a"