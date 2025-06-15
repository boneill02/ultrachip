# See https://www.throwtheswitch.org/build/make

include mk/config/config.mk
include mk/config/config-test.mk

MKDIR = mkdir -p

BUILD_SRC_PATH = $(BUILD_PATH) $(DEPEND_PATH) $(OBJ_PATH) $(RESULTS_PATH)
TEST_SRC = $(wildcard $(TEST_PATH)/*.c)

RESULTS = $(patsubst $(TEST_PATH)/test_%.c,$(RESULTS_PATH)/test_%.txt,$(TEST_SRC) )

PASSED = `grep -s PASS $(RESULTS_PATH)/*.txt`
FAIL = `grep -s FAIL $(RESULTS_PATH)/*.txt`
IGNORE = `grep -s IGNORE $(RESULTS_PATH)/*.txt`

test: $(BUILD_SRC_PATH) $(RESULTS)
	@echo "-----------------------IGNORES:-----------------------"
	@echo "$(IGNORE)"
	@echo "-----------------------FAILURES:-----------------------"
	@echo "$(FAIL)"
	@echo "-----------------------PASSED:-----------------------"
	@echo "$(PASSED)"
	@echo "DONE"
	rm $(RESULTS)

$(RESULTS_PATH)/%.txt: $(BUILD_PATH)/%
	-./$< > $@ 2>&1

$(BUILD_PATH)/test_%: $(OBJ_PATH)/test_%.o $(OBJ_PATH)/unity.o
	$(LINK) -o $@ $^

$(OBJ_PATH)/%.o:: $(TEST_PATH)/%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(OBJ_PATH)/%.o:: $(SRC_PATH)/%.c
	$(COMPILE) $(CFLAGS) $< -o $@

$(OBJ_PATH)/%.o:: $(UNITY_PATH)/%.c $(UNITY_PATH)/%.h
	$(COMPILE) $(CFLAGS) $< -o $@

$(BUILD_PATH):
	$(MKDIR) $(BUILD_PATH)

$(OBJ_PATH):
	$(MKDIR) $(OBJ_PATH)

$(RESULTS_PATH):
	$(MKDIR) $(RESULTS_PATH)

.PRECIOUS: $(BUILD_PATH)/test_/%
.PRECIOUS: $(OBJ_PATH)/%.o
.PRECIOUS: $(RESULTS_PATH)/%.txt
