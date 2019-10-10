
#include "beep.h"
#include "delay.h"
#include "timer.h"

int main(){
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	BEEP_Init();
	BEEP_Play(200);
	TIM6_Init(5000-1,7200-1);
	
	while(1){
		
	}
}
