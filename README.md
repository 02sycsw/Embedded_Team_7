# Embedded_Team_7
Repository for Embedded Project

시연 영상: https://youtube.com/shorts/OWIvqjtxvhY

**[ source directory ]**

Makefile: 디렉터리 내에 있는 소스 파일을 일괄적으로 빌드할 수 있는 규칙 포함

func.h: 필요한 헤더 파일, 상수, 큐를 포함한 구조체, 함수 헤더, 전역 변수 선언

game.c: 
i2c.c:
joy.c:
nfc.c:
queue.c:

spk.c: 
srvMt.c: PWM을 이용하여 서보 모터를 구동시키는 코드. 서보 모터의 초기화 및 각도에 따라 서보 모터를 회전시키는 함수를 구현. 
  : initServo - 서보 모터를 초기화하고 기본 설정을 함.
  : setServoAngle - 서보 모터를 특정 각도로 이동. 0~180인지 확인 후 PWM 값을 계산하고 서보 모터를 움직임. 

**For Test**

rhythm_game.c: 버튼 입력에 따라 음악 노트를 큐로 처리하여 LCD 백라이트의 RGB 값을 제어하는 프로그램.
  : Queue 관련 함수들 - 음악 배열에서 데이터를 가져와 최대 16개의 데이터를 저장하며 순차적으로 삽입(enqueue) 및 제거(dequeue) 가능.
    music 배열의 데이터(1~8)을 큐에 mapping하여 각 값을 문자(LRUD)로 변환. 
  : progressQueue: 음악 배열에서 데이터를 하나씩 큐로 이동시키고 화면에 표시.
  
  : inputThread: 입력 처리 스레드. 버튼 입력 상태를 관리하며 버튼이 눌리면 flag를 설정. 디바운싱을 위해 딜레이를 추가해둠.
  : LCD와 RGB 제어
    LCD는 I2C 통신을 통해 화면을 제어하고, 큐 상태를 받아와서 출력.
    LCD 백라이트의 RGB 값을 변경시켜 제어. 
      성공 시 - 녹색(0, 255, 0) / 실패 시 붉은색(255, 0, 0) / 기본 상태는 푸른색(0, 0, 255)
      
  : 주요 루프
    메인 루프 - 콘솔과 LCD에 현재 queue 상태 출력.
                input_flag를 확인하여 현재 queue 스트라이크존 문자와 비교한 후, 성패에 따라 LCD 백라이트의 RGB 값을 변경.
                0.25초 대기 후 반복. 
    종료 조건 - 준비된 음악 배열이 끝나면 프로그램 종료.
    
test.c: pwm 테스트를 위한 코드
