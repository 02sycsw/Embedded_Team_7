# Embedded_Team_7
Repository for Embedded Project
<br>
<br>
프로젝트 명: 밀어서 잠금해제
<br>
<br>
시연 영상: https://youtube.com/shorts/OWIvqjtxvhY
<br>
<br>
<br>
**[ Sensors and Actuators Used ]** <br>
서보 모터, 조이스틱, ADC 컨버터, LCD 디스플레이, NFC reader, 수동 부저
<br>
<br>
<br>
**[ How to Compile ]** <br>
make
<br>
<br>
<br>
**[ Execute ]** <br>
동작을 시작할 때 - ./game
문을 닫을 때 - ./close
<br>
<br>
<br>
**[ How to Use ]** <br>
1. bash에서 game 파일을 실행시킨 뒤, NFC 칩을 NFC 센서에 태그하여 게임 시작.
2. LCD 화면에 나오는 노트를 보고 조이스틱을 움직임.
   L(eft) / R(ight) / U(p) / D(own) 에 맞춰서 스트라이크존에 노트가 지나갈 때 타이밍을 맞춰서 플레이.
3. 점수 threshold인 40점을 넘기면 서보 모터에 연결된 문이 열림.
4. 안에 있는 리워드를 꺼내고 나면 close 파일을 bash에서 실행시켜 문을 닫음.
<br>
<br>
<br>
<br> 
**[ Source Directory ]**
<br>
**Final Source Code** <br>
<br>
Makefile: 디렉터리 내에 있는 소스 파일을 일괄적으로 빌드할 수 있는 규칙 포함
<br>
func.h: 필요한 헤더 파일, 상수, 큐를 포함한 구조체, 함수 헤더, 전역 변수 선언
<br>
game.c: NFC, 조이스틱, 부저, LCD와 같은 센서와 액추에이터를 제어 하여 리듬 게임을 구현.<br>
  : NFC - NFC reader를 초기화하여 태그 감지를 대기하고, 태그를 감지하면 UID를 읽어옴.<br>
  : 멀티스레드 - 조이스틱 입력을 처리하는 input용 스레드 / 소리(부저) 관련으로 처리하는 output용 스레드<br>
                 입출력을 병렬로 처리하기 위해 2개의 스레드 생성.<br>
  : 음악 처리 - 음악 데이터를 queue로 관리하고, 입력과 비교하여 성패를 판단.<br>
  : 점수 - 스트라이크존 비교를 통해 점수를 계산하고 최종 점수를 LCD에 출력<br>
           일정 점수 이상일 경우 추가 프로그램(open)를 실행.<br>
  : 종료 - 음악 데이터를 모두 처리하면, 스레드를 정리하고 프로그램이 종료.<br>
joy.c: I2C 통신을 이용하여 조이스틱의 입력 데이터를 읽고, 해당 입력에 따라 플래그를 설정하는 프로그램.<br>
nfc.c: NFC의 UID를 감지하고 UID를 문자열로 변환하는 프로그램.<br>
queue.c: LCD와 queue를 활용하여 음악 노트와 조이스틱 입력을 비교하여 점수를 계산하고 표시하는 프로그램. <br>
spk.c: 부저로 주파수에 해당하는 특정 음을 출력하는 프로그램. <br>
       음계와 노트를 비교하여 주파수를 가져온다.<br>
srvMt.c: PWM을 이용하여 서보 모터를 구동시키는 코드. 서보 모터의 초기화 및 각도에 따라 서보 모터를 회전시키는 함수를 구현. <br>
  : initServo - 서보 모터를 초기화하고 기본 설정을 함.<br>
  : setServoAngle - 서보 모터를 특정 각도로 이동. 0 ~ 180인지 확인 후 PWM 값을 계산하고 서보 모터를 움직임. <br>
open.c: 문을 여는 서보모터 동작을 위한 코드. <br>
close.c: 문을 닫는 서보모터 동작을 위한 코드. <br> 
<br>
<br>
<br>
<br>
**For Test** <br>
<br>
i2c.c: NFC 태그를 감지하고, 태그의 UID를 읽는 코드. <br>
rhythm_game.c: 버튼 입력에 따라 음악 노트를 큐로 처리하여 LCD 백라이트의 RGB 값을 제어하는 프로그램. <br>
  : Queue 관련 함수들 - 음악 배열에서 데이터를 가져와 최대 16개의 데이터를 저장하며 순차적으로 삽입(enqueue) 및 제거(dequeue) 가능. <br>
    music 배열의 데이터(1 ~ 8)을 큐에 mapping하여 각 값을 문자(LRUD)로 변환. <br>
  : progressQueue: 음악 배열에서 데이터를 하나씩 큐로 이동시키고 화면에 표시.<br>
  <br>
  : inputThread: 입력 처리 스레드. 버튼 입력 상태를 관리하며 버튼이 눌리면 flag를 설정. 디바운싱을 위해 딜레이를 추가해둠.<br>
  : LCD와 RGB 제어<br>
    LCD는 I2C 통신을 통해 화면을 제어하고, 큐 상태를 받아와서 출력.<br>
    LCD 백라이트의 RGB 값을 변경시켜 제어. <br>
      성공 시 - 녹색(0, 255, 0) / 실패 시 붉은색(255, 0, 0) / 기본 상태는 푸른색(0, 0, 255)<br>
   <br>   
  : 주요 루프
    메인 루프 - 콘솔과 LCD에 현재 queue 상태 출력.
                input_flag를 확인하여 현재 queue 스트라이크존 문자와 비교한 후, 성패에 따라 LCD 백라이트의 RGB 값을 변경.
                0.25초 대기 후 반복. 
    종료 조건 - 준비된 음악 배열이 끝나면 프로그램 종료.
