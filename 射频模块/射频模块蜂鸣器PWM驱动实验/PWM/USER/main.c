
#include "beep.h"
#include "delay.h"
#include "timer.h"

int main(){
	delay_init();
	BEEP_Init();
	TIM3_PWM_Init(BEEP_DRIVER);
	BEEP_Play(1000,BEEP_INTERVAL);
	while(1){
		delay_ms(1000);
		BEEP_Play(1000,BEEP_INTERVAL);
	}
}
