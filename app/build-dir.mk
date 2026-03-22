##
## Out-of-tree build: .o, .d, and firmware outputs go to build/
## Include this AFTER rules.mk in Makefile.include
##

BUILD_DIR ?= build

# Override OBJS to use build/
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))


# Override targets to output to build/
elf: $(BUILD_DIR)/$(BINARY).elf
bin: $(BUILD_DIR)/$(BINARY).bin
hex: $(BUILD_DIR)/$(BINARY).hex
srec: $(BUILD_DIR)/$(BINARY).srec
list: $(BUILD_DIR)/$(BINARY).list

GENERATED_BINARIES := $(BUILD_DIR)/$(BINARY).elf $(BUILD_DIR)/$(BINARY).bin \
	$(BUILD_DIR)/$(BINARY).hex $(BUILD_DIR)/$(BINARY).srec \
	$(BUILD_DIR)/$(BINARY).list $(BUILD_DIR)/$(BINARY).map


# Compile .c -> build/*.o with .d in build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@printf "  CC      $<\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(filter-out -MD,$(TGT_CPPFLAGS)) $(CPPFLAGS) \
		-MMD -MF $(BUILD_DIR)/$*.d -MT $@ -o $@ -c $<

# Override clean to remove build dir
clean:
	@printf "  CLEAN\n"
	$(Q)$(RM) -rf $(BUILD_DIR)
	$(Q)$(RM) -f generated.*

# Override flash to use build/firmware.elf
$(BUILD_DIR)/$(BINARY).flash: $(BUILD_DIR)/$(BINARY).elf
	@printf "  FLASH   $<\n"
	(echo "halt; program $(realpath $<) verify reset" | nc -4 localhost 4444 2>/dev/null) || \
		$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
		-f target/$(OOCD_TARGET).cfg \
		-c "program $(realpath $<) verify reset exit" \
		$(NULL)

flash: $(BUILD_DIR)/$(BINARY).flash

# Include dependency files from build/
-include $(OBJS:.o=.d)