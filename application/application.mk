# Application build rules

# Application source files
APP_SRC_DIR := application
APP_SRCS := $(wildcard $(APP_SRC_DIR)/*.c)
APP_OBJS := $(patsubst $(APP_SRC_DIR)/%.c, $(APP_OUTPUT_DIR)/%.o, $(APP_SRCS))

# Application executable
APP_EXEC := $(DIST_DIR)/cmocka-app

# Application specific flags (use installed SDK from build directory)
APP_CFLAGS := $(CFLAGS) -I$(SDK_INSTALL_INC_DIR)
APP_LDFLAGS := -L$(SDK_INSTALL_LIB_DIR) -lsdk

# Build application (depends on sdk_install)
.PHONY: app
app: sdk_install $(APP_EXEC)

$(APP_EXEC): $(APP_OBJS)
	@echo "Building application: $@"
	@$(MKDIR) $(dir $@)
	$(CC) $(APP_OBJS) -o $@ $(APP_LDFLAGS)
	@echo "Application built successfully: $@"

# Compile application source files
$(APP_OUTPUT_DIR)/%.o: $(APP_SRC_DIR)/%.c
	@echo "Compiling: $<"
	@$(MKDIR) $(dir $@)
	$(CC) $(APP_CFLAGS) -c $< -o $@

# Run the application
.PHONY: run
run: app
	@echo "======================================"
	@echo "Running application..."
	@echo "======================================"
	@$(APP_EXEC)

# Clean application artifacts
.PHONY: clean-app
clean-app:
	$(RM) $(APP_OUTPUT_DIR) $(APP_EXEC)