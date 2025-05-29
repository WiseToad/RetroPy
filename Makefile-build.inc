TARGET = $(BUILD_DIR)/$(TARGET_$(OS))

OBJS = $(addprefix $(BUILD_DIR)/, $(SOURCE:.cpp=.o))

.PHONY: all clean distclean

all: $(TARGET)

clean:
	rm -f $(OBJS)

distclean:
	rm -f $(OBJS) $(TARGET)

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)/%/.

$(BUILD_DIR)/.:
	@mkdir -p $@

$(BUILD_DIR)/%/.:
	@mkdir -p $@

$(TARGET): $(OBJS)
	$(LNK) $(LNKFLAGS) $(LNKFLAGS.$(BUILD)) $(OBJS) $(LIBS) -o $@

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: src/%.cpp | $$(@D)/.
	$(CXX) $(CXXFLAGS) $(CXXFLAGS.$(BUILD)) $(INCLUDE) -c $< -o $@
