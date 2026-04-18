CXX ?= g++
CXXFLAGS_COMMON := -std=c++17 -Wall -Wextra

INCLUDES := -Iinclude -Ilib -Ilib/hash-lib

ifeq ($(OS),Windows_NT)
    TARGET := thulogin.exe
    LIBS := -lwsock32 -lws2_32
else
    TARGET := thulogin
    LIBS :=
endif

SRCS := src/main.cpp \
        src/encrypt/SrunBase64.cpp \
        src/encrypt/SrunMd5.cpp \
        src/encrypt/SrunSHA1.cpp \
        src/encrypt/SrunXEncode.cpp \
        src/Authenticator.cpp \
        src/PwdInput.cpp \
        lib/hash-lib/md5.cpp \
        lib/hash-lib/sha1.cpp \
        lib/Encoder.cpp

OBJS_RELEASE := $(SRCS:.cpp=.o)
OBJS_DEBUG := $(SRCS:.cpp=.dbg.o)

.PHONY: all debug clean

all: $(TARGET)

$(TARGET): $(OBJS_RELEASE)
	$(CXX) $(CXXFLAGS_COMMON) -O2 -DNDEBUG $(INCLUDES) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS_COMMON) -O2 -DNDEBUG $(INCLUDES) -c -o $@ $<

debug: $(OBJS_DEBUG)
	$(CXX) $(CXXFLAGS_COMMON) -O0 -g $(INCLUDES) -o $(TARGET) $^ $(LIBS)

%.dbg.o: %.cpp
	$(CXX) $(CXXFLAGS_COMMON) -O0 -g $(INCLUDES) -c -o $@ $<

clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\\,$(OBJS_RELEASE)) $(subst /,\\,$(OBJS_DEBUG)) $(subst /,\\,$(TARGET)) 2>nul || true
else
	$(RM) $(OBJS_RELEASE) $(OBJS_DEBUG) $(TARGET)
endif
