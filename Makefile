# Compiler and Compile options.
CC = g++
CXXFLAGS = -g -Wall -std=c++11

# Macros specifying path for compile.
SRCS := hw1.cpp
# SRCS에서 cpp를 o로 치환
OBJS := $(SRCS:.cpp=.o)
INC = ./include/
# POSIX 라이브러리
LIB = ./lib/ -lpthread

# Pre-Processor.
CPPFLAGS += -I$(INC)

# Compile command.
# 타겟명 : 선행 파일 목록 (rule), 여러 개의 룰이 존재할 수 있음
# 타겟명은 룰의 목적
# 타겟을 빌드하는데 사용됨
TARGET = run
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJS) -L$(LIB)

# Delete binary & object files.
clean:
	rm $(TARGET) $(OBJS)
