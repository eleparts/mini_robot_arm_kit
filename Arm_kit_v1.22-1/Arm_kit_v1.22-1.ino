/*************************************************************
* Robot Arm 제어 프로그램
* 수정 : 2017. 08. 31
* 제작 : eleparts 부설연구소
* SW ver. 1.22
* Servo 모터 4개 + 조이스틱(analog data) + SW 4개 제어 + 기능 SW / 역방향
**************************************************************/
#include <Servo.h>


/* MG90 모터의 동작 각도 (0~180도)*/
#define MAX_ANGLE 180        // SG90 모터 제어 최대 각도 (-6도)
#define MID_ANGLE 90         // SG90 모터 각도 중간값
#define MIN_ANGLE 0          // SG90 모터 제어 최소 각도 (+6도)

#define MIN_STICK 80         // 조이스틱 analog신호 인식 범위 조정(민감도 설정 - 중간값 : 500~520 )
#define MAX_STICK 943   

/* servo 모터별 동작 주파수(Duty Cucle) 관련링크 : http://blog.naver.com/elepartsblog/221061007108 */
#define MIN_MG90 680         // MAX 1000
#define MAX_MG90 2300        // MIN 2000
#define MIN_MG995 540      
#define MAX_MG995 2440   
/* Hand Servo 모터 각도 조정 */
#define MIN_HAND 800          // 650~1000 사이 숫자로 프로그램을 업로드했을때 손 모터에서 잡음이 나지 않고 딱 물리도록 설정해 줍니다.
#define MAX_HAND 2100   

#define TRUE 1               // 기본 정의
#define FALSE 0
#define FRONT 1
#define BACK 0

const int pin_Joy1_X = 1;    // 1번 조이스틱 X축 analog pin 번호
const int pin_Joy1_Y = 0;    // 1번 조이스틱 Y축 analog pin 번호
const int pin_Sw1_X  = 6;    // Sw A pin 번호
const int pin_Sw1_Y  = 4;    // Sw C pin 번호
const int pin_Sw2_X  = 7;    // Sw D pin 번호
const int pin_Sw2_Y  = 5;    // Sw B pin 번호

const int pinHand   = 8;     //  손 모터 pin 번호
const int pinArmTop = 9;     //  팔 - 상단 모터 pin 번호
const int pinArmBot = 10;    //  팔 - 하단 모터 pin 번호
const int pinProp   = 11;    //  받침 모터 pin 번호

const int SubsysSwA = 3;     // 보조 옵션(평행이동)기능 스위치 pin 번호  

Servo Hand;
Servo ArmTop; 
Servo ArmBot; 
Servo Prop; 

int anValue_x1, anValue_y1;                             // 조이스틱 analog 데이터 저장용 변수
int swValue_1x, swValue_1y, swValue_2x, swValue_2y;     // SW 데이터 저장용 변수
int swSubA = 1;                       
                  
/* 각도 저장용 변수 : 90도로 초기설정 // Hand 부분만 0도(닫힌상태) */
int angleHand = MIN_ANGLE;                              
int angleArmTop = MID_ANGLE;
int angleArmBot = MID_ANGLE;
int angleProp = MID_ANGLE;


/* 사용자 함수 정의 - 아두이노 스케치에서는 크게 필요없지만 컴파일러에 따라서 위에 함수원형 선언이 필요하기도 함 */
void readPin();
int valueRange(int stickValue, int angle);
int swValueRange(int xSwValue, int ySwValue, int angle);
void angle();



/* 최초 부팅시 1회 실행되는 아두이노 스케치용 함수 */
void setup() {  
  /* 각 핀번호 입력*/
  Hand.attach(pinHand, MIN_HAND, MAX_HAND );                       // 서보모터 
  ArmTop.attach(pinArmTop, MIN_MG995, MAX_MG995);
  ArmBot.attach(pinArmBot, MIN_MG995, MAX_MG995); 
  Prop.attach(pinProp, MIN_MG90, MAX_MG90);
     
  pinMode(pin_Sw1_X, INPUT_PULLUP);            // 스위치(SW) 핀 입력으로 설정
  pinMode(pin_Sw1_Y, INPUT_PULLUP);
  pinMode(pin_Sw2_X, INPUT_PULLUP);
  pinMode(pin_Sw2_Y, INPUT_PULLUP);
  pinMode(SubsysSwA, INPUT_PULLUP); 

  /* 초기 설정 : 90도로 모터 제어 */
  Hand.write(angleHand);
  ArmTop.write(angleArmTop);
  ArmBot.write(angleArmBot);
  Prop.write(angleProp);

  delay(100);
}


/* 반복 실행되는 아두이노 스케치용 함수 */
void loop() {

  readPin();                      // 조이스틱의 및 스위치의 상태 값을 읽어오는 함수                  (입력)

  if(swSubA == FALSE){              // 입력 직후 보조기능 동작유무를 먼저 확인해줌
    parallelMovement();
    delay(500);
  }

  angle();                         // 모터 제어 함수 - 입력받은 값을 이용해 각도를 연산해주는 함수    (연산)

  /* 수정된 각도를 이용해 모터를 움직여 줌 */ //                                                    (출력/제어)
  Hand.write(angleHand);
  ArmTop.write(angleArmTop);
  ArmBot.write(angleArmBot);
  Prop.write(angleProp);

  delay(8);                         // 딜레이 ms 초 (동작 반응/지연속도)
}


/*********************** 기본 기능 함수 ***********************/

/* 조이스틱 및 스위치의 상태를 받아오는 함수 ( 조이스틱 analog 값 : 0~1023을 저장함 ) */
void readPin()
{
  anValue_x1 = analogRead(pin_Joy1_X);                           // 조이스틱 (아날로그) Read
  anValue_y1 = analogRead(pin_Joy1_Y);

  swValue_1x = digitalRead(pin_Sw1_X);                         // 스위치 (디지털) Read
  swValue_1y = digitalRead(pin_Sw1_Y);
  swValue_2x = digitalRead(pin_Sw2_X);
  swValue_2y = digitalRead(pin_Sw2_Y);

  swSubA = digitalRead(SubsysSwA);
}

/* 조이스틱으로 모터의 각도를 조정하는 함수 */
int valueRange(int stickValue, int angle)         
{
  if(stickValue < MIN_STICK && angle < MAX_ANGLE){            // 조이스틱 신호가 X방향 입력됨 && 현재 각도가 MAX보다 적을때 
    angle = angle + 1;                                        // -> 각도를 올려줌
  }else if(stickValue > MAX_STICK && angle > MIN_ANGLE){      // 조이스틱 신호가 Y방향 입력됨 && 현재 각도가 MIN보다 클때   
    angle = angle - 1;                                        // -> 각도를 내려줌
  }
  return angle;
}

/* 스위치 두개로 모터의 각도를 조정하는 함수 */
int swValueRange(int xSwValue, int ySwValue, int angle) 
{
  if(xSwValue == FALSE && ySwValue == TRUE && angle < MAX_ANGLE){                 // 스위치 X가 입력됨 &&  현재 각도가 MAX보다 적을때 
    angle = angle + 1;                                        // -> 각도를 올려줌
  }else if(ySwValue == FALSE && xSwValue == TRUE && angle > MIN_ANGLE){           // 스위치 Y가 입력됨 &&  현재 각도가 MIN보다 클때 
    angle = angle - 1;                                        // -> 각도를 내려줌
  } 
  return angle;
}


/* 모터의 각도값을 연산하는 함수들을 호출해주는 함수 */
void angle()
{
  angleProp = valueRange(anValue_x1, angleProp);
  angleArmBot = valueRange(anValue_y1, angleArmBot);
  angleArmTop = swValueRange(swValue_1y, swValue_1x, angleArmTop);
  angleHand = swValueRange(swValue_2y, swValue_2x, angleHand);
}





/*********************** 보조기능 - 평행이동 함수 ***********************/

/* 자동 제어중에 중지 SW 입력을 받기 위해 SW 입력 기능이 추가된 delay문 */
int delay_SwCheck(int time)                     
{
  for(int i = 0; i<time ; i++)
  {
    if(!digitalRead(SubsysSwA)){                     // delay중에 bs SW가 입력되면 동작을 정지한 뒤
       return 1;                                     // 1 을 반환한다. - 호출시 1을 반환받으면 해당 함수를 종료하도록 구현함
    }
    delay(1);
  } 
  return 0;
}

/* 평행이동 제어함수 */
void moveAngle(int moveAngleValue, int direction)
{
  int i;

  for(i=0; i< moveAngleValue; i++){
    if(direction == FRONT)
    {  
      angleArmTop--;              // 전진
      angleArmBot--;
    }else{
      angleArmTop++;              // 후진
      angleArmBot++;
    }
    ArmTop.write(angleArmTop);
    ArmBot.write(angleArmBot);
    
    if(delay_SwCheck(8)){         // 지연
      break;
    }
  }
}



/* 평행이동 범위(각도) 계산 함수 */
int MoveRange(int rtol)
{
  int TopAngle, botAngle, moveAngle;
 
  if(rtol)
  {
    TopAngle = abs(MAX_ANGLE - angleArmTop);                   // abs : 절대값 함수 (음수 제거)
    botAngle = abs(MAX_ANGLE - angleArmBot);

  }else{
    TopAngle = abs(MIN_ANGLE - angleArmTop);
    botAngle = abs(MIN_ANGLE - angleArmBot); 
  }

  moveAngle = (TopAngle > botAngle) ? botAngle : TopAngle ;    // 3항 연산자 - 두 모터의 이동가능한 각도중에서 적은쪽을 반환

  return moveAngle;
}



/* 평행이동 함수 */
void parallelMovement(){

  int FrontMoveAngle, backMoveAngle, moveAngleValue, i;

  delay(800);
  
  backMoveAngle  = MoveRange(TRUE);                    // 뒤로 이동 가능한 각도
  FrontMoveAngle = MoveRange(FALSE);                   // 앞으로 이동 가능한 각도

  moveAngleValue = FrontMoveAngle + backMoveAngle;     // 앞 + 뒤로 이동가능한 각도 

  moveAngle(backMoveAngle, BACK);                      // 초기 이동 - 두개 모터의 최대 허용 각도까지 뒤로 이동 
  if(delay_SwCheck(1000)){
    return;
  }

  while(1){

    moveAngle(moveAngleValue, FRONT);                   // 앞으로
    if(delay_SwCheck(500)){
      return;
    }

    moveAngle(moveAngleValue, BACK);                    // 뒤로
    if(delay_SwCheck(1000)){
      return;
    }
  }
}



