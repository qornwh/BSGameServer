# Makefile

# 컴파일러 설정
CC = g++
# 컴파일러 플래그 설정
CFLAGS = -Wall
# 라이브러리 파일 이름 설정
LIBRARY_NAME = libcore.a

# 소스 코드 디렉토리 설정
SRC_DIR = ./
# 빌드 디렉토리 설정
BUILD_DIR = ../lib/CoreLib
# 빌드 디렉토리의 include 서브디렉토리 경로
INCLUDE_DIR = $(BUILD_DIR)/include

# 소스 코드 파일 확장자 설정
SOURCE_EXT = cpp
# 헤더 파일 확장자 설정
HEADER_EXT = h

# 소스 코드 파일 검색
SOURCES = $(wildcard $(SRC_DIR)/*.$(SOURCE_EXT))
# 헤더 파일 검색
HEADERS = $(wildcard $(SRC_DIR)/*.$(HEADER_EXT))
# 개체 파일 생성
OBJECTS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SOURCE_EXT)=.o))

# 기본 대상: 라이브러리 빌드
all: $(BUILD_DIR)/$(LIBRARY_NAME) copy_headers

# 라이브러리 빌드 규칙
$(BUILD_DIR)/$(LIBRARY_NAME): $(OBJECTS)
	ar rcs $@ $^

# 개체 파일 빌드 규칙
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SOURCE_EXT) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

# 헤더 파일 복사 규칙
copy_headers: $(HEADERS)
	@mkdir -p $(INCLUDE_DIR)
	cp $(HEADERS) $(INCLUDE_DIR)

# 정리 규칙
clean:
	rm -rf $(BUILD_DIR)

# 실행 파일 생성
executable: $(OBJECTS)
	$(CC) $^ -o $(BUILD_DIR)/your_executable_name
