# 신호등 인식 자율 주행 차량

## Topic & Idea
1. Touch 센서 인식되면 Buzzer 울리면서 주행 시작 알림
2. 웹캠이 켜지고, TFLite를 이용한 객체 인식
3. 인식된 신호등에 따라 Motor 동작                   
4. 차선을 인식하고, 소실점을 활용하여 차선을 벗어나지 않도록 차량 제어

	- case1. 초록불 신호등이면 직진  
	- case2. 빨간불 신호등이면 정지하고 빨간색 LED 켜짐  
	- case3. 좌회전 신호등이면 왼쪽 노란색 LED 깜빡이고, 좌회전 함  