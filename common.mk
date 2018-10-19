# 目标配置
TARGET  := $(strip $(TARGET))
TYPE    := $(strip $(TYPE))
INSTALL_DIR := $(strip $(INSTALL_DIR))

# 编译器工具
CC       = gcc
CXX      = g++
CPP      = cpp

# 编译器选项
CFLAGS	 += -Wall #-Werror
CXXFLAGS += -Wall #-Werror
CXXFLAGS += -std=c++0x -Wno-pmf-conversions 

CFLAGS	 += $(EXTRA_CFLAGS)
CXXFLAGS += $(EXTRA_CXXFLAGS)

# 包含选项 include
INC_DIR  += $(COMM_INC_DIR)
INC_DIR  += /usr/include/mysql/ /usr/include/ /usr/local/mysql/include/mysql/ /usr/include/libxml2/ ../ ./ ../../
C_INC    = $(INC_DIR:%=-I%)
CFLAGS   += $(C_INC)
CXXFLAGS += $(C_INC)

# 链接选项 lib
LIB_DIR  += $(COMM_LIB_DIR)
LIB_DIR += /usr/lib/ /usr/lib64/ /usr/lib64/mysql/
SYS_LIB += libprotobuf libpthread librt libz libssl libcurl libxml2 libmysqlclient libcrypto libiconv
C_LIB    = $(LIB_DIR:%=-L%) $(LIB_DEPENDS:lib%=-l%) $(SYS_LIB:lib%=-l%)

# 源文件处理
SRC_DIR  += .
C_SRC   = $(wildcard $(SRC_DIR:%=%/*.c))
CXX_SRC = $(wildcard $(SRC_DIR:%=%/*.cpp))
CCC_SRC = $(wildcard $(SRC_DIR:%=%/*.cc))
C_OBJ   = $(C_SRC:%.c=%.o)
CCC_OBJ = $(CCC_SRC:%.cc=%.o)
CXX_OBJ = $(CXX_SRC:%.cpp=%.o)
C_DEP   = $(C_SRC:%.c=%.d)
CCC_DEP = $(CCC_SRC:%.cc=%.d)
CXX_DEP = $(CXX_SRC:%.cpp=%.d)

ALL_OBJ = $(C_OBJ) $(CXX_OBJ) $(CCC_OBJ)
ALL_DEP = $(C_DEP) $(CXX_DEP) $(CCC_DEP)
# ALL_GCNO = $(ALL_OBJ:%.o=%.gcno)

TARGET_INSTALL = $(INSTALL_DIR)/$(TARGET)
SO_DEPENDS = $(LIB_DEPENDS:%=$(COMM_LIB_DIR)/%.so)

debug=1
ifeq ($(debug), 1)
CFLAGS   += -g -DDEBUG_GLOBAL
CXXFLAGS += -g -DDEBUG_GLOBAL
# LDFLAGS  += --coverage
else
CFLAG += -DNDEBUG
CXXFLAGS += -DNDEBUG
endif

.PHONY: all clean install

all:$(TARGET)

clean:
	rm -rf $(ALL_OBJ) $(ALL_DEP) $(TARGET) $(ALL_GCNO)

# 每个目标的包含依赖处理
$(C_DEP): %.d : %.c
	$(CPP) $(EXTRA_CFLAGS) $(C_INC) -MM $< > $@
$(CXX_DEP): %.d : %.cpp
	$(CPP) $(EXTRA_CXXFLAGS) $(C_INC) -MM $< > $@
$(CCC_DEP): %.d : %.cc
	$(CPP) $(EXTRA_CXXFLAGS) $(C_INC) -MM $< > $@

ifneq ($(MAKECMDGOALS), clean)
-include $(ALL_DEP)
endif

# 编译目标
ifeq ($(TYPE), app)
$(TARGET): $(ALL_DEP) $(ALL_OBJ) $(OBJ_DEPENDS)
	$(CXX) $(C_LIB) $(ALL_OBJ) -Wl,--start-group $(OBJ_DEPENDS) -Wl,--end-group $(LDFLAGS) -o $@ -g
install: all
	cp -f $(TARGET) $(TARGET_INSTALL)
else
ifeq ($(TYPE), lib)
$(TARGET):$(ALL_DEP) $(ALL_OBJ) $(OBJ_DEPENDS)
	$(AR) r $(TARGET) $(ALL_OBJ)
	cp -f $(TARGET) $(TARGET_INSTALL)
install:all
	cp -f $(TARGET) $(TARGET_INSTALL)
else
ifeq ($(TYPE), so)
CFLAGS   += -fPIC
CXXFLAGS += -fPIC
$(TARGET): $(ALL_DEP) $(ALL_OBJ) $(SO_DEPENDS) $(OBJ_DEPENDS)
	$(CXX) -shared -fPIC $(C_LIB) $(ALL_OBJ) -Wl,--start-group $(OBJ_DEPENDS) -Wl,--end-group $(LDFLAGS) -o $@
install: all
	cp -f $(TARGET) $(TARGET_INSTALL)
else
ifeq ($(TYPE), obj)
.PHONY: $(TARGET)
$(TARGET): $(ALL_DEP) $(ALL_OBJ)
install: all
else
$(error $$(TYPE) should be 'app', 'so' or 'obj', but not '$(TYPE)')
endif
endif
endif
endif
