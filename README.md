# Linux Epoll을 이용한 온라인 게임 서버 구현

## 구현 목록

1. 플레이어 다중 접속
2. 플레이어 다중 이동
3. 플레이어 다중 채팅
4. 같은 지역 Jobqueue 구현

## 추가 목표 

1. 몬스터 구현 => 이동 공격 명령 서버에서 처리
2. 플레이어 공격 
3. 맵정보 설정 => x, y좌표 지정

## 사용법 

### DEBUG

1. liunx gcc 컴파일러 경로 지정 (현재 디폴트 경로)
2. GameBS에서 main.cpp파일에서 f5 또는 실행 및 DEBUG 탭에서 빌드용 실행

### RELEASE

1. ./CoreLib 경로에서 make 실행
2. ../ 상위로 올라가서 make 실행
3. 실행되면 build 폴더에 GAMESERVER파일 실행

### core dump 
sudo systemctl disable apport - 디폴트로 설정되어있어서 끈다
sudo sysctl -w kernel.core_pattern=core.%e - 생성될 core dump파일 이름형식을 지정
ulimit -c unlimited - core dump 생성 제한 해제
gdb사용법 분석필요