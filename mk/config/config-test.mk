UNITY_PATH = Unity/src
BUILD_PATH = build
OBJ_PATH = build/objs
RESULTS_PATH = build/results

CFLAGS += -I. -I$(UNITY_PATH) -I$(SRC_PATH) -DTEST
COMPILE = $(CC) -c $(CFLAGS)
LINK= $(CC)
