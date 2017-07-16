##
## The main makefile
##
## author    Oshyn Song
## email     dualyangsong@gmail.com
##

DEBUG=0
INCLUDE_PATH=-I$(CURDIR)/src
LIB_PATH=-L/usr/lib
LIB=-lpthread

CXXFLAGS=-Wall -pipe -std=c++11 
SHARED_FLAGS=-fPIC -shared
ifeq ($(DEBUG), 1)
	CXXFLAGS+=-g -O0
else
	CXXFLAGS+=-O3
endif
CXX=g++

OUT_PATH=$(CURDIR)/output
PROJECT=threadpool
SRC_PATH=$(CURDIR)/src
SOURCES:=$(foreach SRC,$(SRC_PATH),\
	$(wildcard $(SRC)/*.cpp)\
)
STATIC_OBJECTS:=$(foreach S,$(SOURCES),\
	$(addprefix $(OUT_PATH)/,\
		$(patsubst %.cpp,%.o,\
			$(subst $(SRC_PATH)/,,$(S))\
		)\
	)\
)
SHARED_OBJECTS:=$(foreach S,$(SOURCES),\
	$(addprefix $(OUT_PATH)/,\
		$(patsubst %.cpp,%.lib,\
			$(subst $(SRC_PATH)/,,$(S))\
		)\
	)\
)
STATIC=libthreadpool.a
SHARED=libthreadpool.so
TEST=threadpooltest

# Starting here to construct
.PHONY: all
all: static shared test

.PHONY: static
static: $(STATIC)
$(STATIC): \
	$(OUT_PATH)/task.o \
	$(OUT_PATH)/util.o \
	$(OUT_PATH)/thread.o \
	$(OUT_PATH)/threadpool.o

	@echo "Start building $@..."
	ar -crv $@ $^
	@echo "Build $@ successfully!"

.PHONY: shared
shared: $(SHARED)
$(SHARED): \
		$(OUT_PATH)/task.lib \
		$(OUT_PATH)/util.lib \
		$(OUT_PATH)/thread.lib \
		$(OUT_PATH)/threadpool.lib

	@echo "Start building $@..."
	$(CXX) $(LIB) $(LIB_PATH) -o $@ $^ $(CXXFLAGS) $(SHARED_FLAGS)
	@echo "Build $@ successfully!"

.PHONY: test
test: $(TEST)
$(TEST): \
	$(OUT_PATH)/task.o \
	$(OUT_PATH)/util.o \
	$(OUT_PATH)/thread.o \
	$(OUT_PATH)/threadpool.o \
	$(OUT_PATH)/test.o

	@echo "Start building $@..."
	$(CXX) $^ -o $@ $(LIB) $(LIB_PATH) $(CXXFLAGS)
	@echo "Build $@ successfully!"


$(filter %.o,$(STATIC_OBJECTS)) : $(OUT_PATH)/%.o:$(SRC_PATH)/%.cpp
	@echo "Compiling $@ from $<..."
	@$(shell mkdir -p $(dir $@))
	$(CXX) -c $< -o $@ $(INCLUDE_PATH) $(CXXFLAGS) $(LIB) $(LIB_PATH)

$(filter %.lib,$(SHARED_OBJECTS)) : $(OUT_PATH)/%.lib:$(SRC_PATH)/%.cpp
	@echo "Compiling $@ from $<..."
	@$(shell mkdir -p $(dir $@))
	$(CXX) -c $< -o $@ $(INCLUDE_PATH) $(CXXFLAGS) $(SHARED_FLAGS)


.PHONY : clean
clean:
	@-rm -f $(STATIC) $(SHARED) $(TEST)
	@-rm -rf $(OUT_PATH)
	@echo clean the whole built files!

