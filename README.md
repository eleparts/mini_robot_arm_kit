
# Mini Robot ARM KIT(아두이노 호환)

엘레파츠의 **Mini Robot ARM KIT**의 예제 소스입니다.
Robot ARM KIT의 정보는 아래 링크를 참고해 주세요.
https://www.eleparts.co.kr/EPXJ4AN3



## Arm_kit_vX.XX 폴더

**아두이노 코드 파일**이 버전별로 저장되어 있으며, **다운로드 후 바로 실행이 가능**합니다.

## 소스코드 폴더

**소스 코드**가 버전별로  **.c** 파일로 저장되어 있습니다.


## ARM Kit - menual

사용자  매뉴얼 입니다.

- 제품 구성 및 조립 방법등이 자세히 설명되어 있습니다.

## 버전별 기능과 프로그램 유의사항.txt

스위치 제어 수정 방법에 대한 설명 및  버전별 추가사항이 정리되어 있습니다.

### 버전별 기능 추가 내역

- Arm_kit_v1.00  
기본 제어 기능 코드입니다

- Arm_kit_v1.22 
기본 제어기능 및 평행이동 기능 탑재
	> **평행이동 기능 사용시 주의사항** - 해당 기능이 동작될때 동작 방향이 이상하게 동작할 경우 (앞뒤로 크게 이동하는 경우)  
	> **[평행이동 제어함수]** **[평행이동 범위(각도) 계산 함수]** 함수 아래에 있는 주석을 참고하여 함수를 수정해 주어야 합니다. (모터 방향에 따른 버그입니다.)


- Arm_kit_v1.22-1 
위 평행이동과 방향설정만 반대인 코드입니다.
	> **1.22ver이 오작동 시 코드 수정 대신** 넣어 주시면 됩니다.

- Arm_kit_v1.31 
기본 제어기능 및 평행이동 기능 및 기억-실행 기능이 추가되어 있습니다.

	>**기억 실행 기능** : **F 버튼**을 눌러 현재 동작(위치)를 기억시킵니다. 
	>해당 동작을 최대 [SAVE_DATA_MAX_NUMBER] 횟수만큼 (기본값 30)저장할수 있으며, **E 버튼**을 누르면 지금까지 **저장된 동작을 차례대로 실행**합니다.
	>이때 **E 키**(실행 기능)은 누를때마다 1회 실행되며 중간에 취소나 추가 조작이 불가능합니다.
	>다시 **F 키**를 누르면 저장된 내역이 초기화 되며 첫 번째 동작부터 다시 저장됩니다. (이어서 저장하기 불가능.)


- Arm_kit_v1.32
기억- 실행 기능의 동작 속도를 조절하는 함수가 추가되었습니다.



## 첨부 - 평행이동 수정.txt

평행이동 기능이 비정상적으로 동작할 경우 대처방법입니다.

- 모터의 동작 방향이 반대로 되어 펴짐/접힘 동작을 할 경우 대응 코드로 변경합니다.
<!--stackedit_data:
eyJoaXN0b3J5IjpbMTEzMDY4NTg3Nl19
-->