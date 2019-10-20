
#include "exti.h"
#include "sys.h"

int main(){
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	EXTIx_Init();
	while(1){
		
	}
}

