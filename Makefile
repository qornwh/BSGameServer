# Makefile for Game folder

# 컴파일러 설정
CC = g++
# 컴파일러 플래그 설정
CFLAGS = -c -Wall
# 컴파일러 디버그 플래그 설정
DCFLAGS = -g
# 라이브러리 파일 이름 설정
LIBRARY_NAME = libcore.a

# 빌드 디렉토리 설정
BUILD_DIR = ./build
# 소스 코드 디렉토리 설정 (Game 폴더)
GAME_SRC_DIR = ./GameBS
# 라이브러리 디렉토리 설정 (CoreLib 폴더)
LIB_DIR = ./lib/CoreLib
# 라이브러리 헤더 파일 디렉토리 설정 (CoreLib 폴더)
LIB_INCLUDE_DIR = $(LIB_DIR)/include
# 실행 파일 이름 설정
EXECUTABLE_NAME = GAMESERVER

# 소스 코드 파일 확장자 설정
SOURCE_EXT = cpp
# 헤더 파일 확장자 설정
HEADER_EXT = h

# 소스 코드 파일 검색 (Game 폴더)
GAME_SOURCES = $(wildcard $(GAME_SRC_DIR)/*.$(SOURCE_EXT))
# 헤더 파일 검색 (Game 폴더)
GAME_HEADERS = $(wildcard $(GAME_SRC_DIR)/*.$(HEADER_EXT))
# 개체 파일 생성 (Game 폴더)
GAME_OBJECTS = $(patsubst $(GAME_SRC_DIR)/%,$(BUILD_DIR)/%,$(GAME_SOURCES:.$(SOURCE_EXT)=.o))

# 라이브러리 파일 경로 및 이름 설정
LIBRARY_PATH = $(LIB_DIR)/libcore.a

# 기본 대상: 실행 파일 빌드
all: $(BUILD_DIR)/$(EXECUTABLE_NAME)

# 실행 파일 빌드 규칙
$(BUILD_DIR)/$(EXECUTABLE_NAME): $(GAME_OBJECTS)
	$(CC) $^ -o $@ -L$(LIB_DIR) -lcore -I$(LIB_INCLUDE_DIR) $(DCFLAGS)

# 개체 파일 빌드 규칙
$(BUILD_DIR)/%.o: $(GAME_SRC_DIR)/%.$(SOURCE_EXT) $(GAME_HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(LIB_INCLUDE_DIR) $< -o $@

# 정리 규칙
clean:
	rm -rf $(BUILD_DIR)
