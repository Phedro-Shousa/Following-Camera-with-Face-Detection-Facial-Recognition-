#include "utils.h"
#include <linux/module.h>

void SetGPIOFunction(struct GpioRegisters *s_pGpioRegisters, int GPIO, int functionCode) {
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;

	unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
	unsigned mask = 0b111 << bit;

	//pr_alert("%s: register index is %d\n",__FUNCTION__,registerIndex);
	//pr_alert("%s: mask is 0x%x\n",__FUNCTION__,mask);
	//pr_alert("%s: update value is 0x%x\n",__FUNCTION__,((functionCode << bit) & mask));
	
	s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

void SetGPIOOutputValue(struct GpioRegisters *s_pGpioRegisters, int GPIO, bool outputValue) {

	//pr_alert("%s: register value is 0x%x\n",__FUNCTION__,(1<<(GPIO %32)));
			 
	if (outputValue)
		s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}

bool GetGPIOInputValue(struct GpioRegisters *s_pGpioRegisters, int GPIO){
	
	//pr_alert("%s: register value is 0x%x\n",__FUNCTION__,(1<<(GPIO %32)));

	unsigned mask = 1 << (GPIO % 32); //mask used to clear all the bits that doesn't matter

	bool value=(s_pGpioRegisters->GPLEV[GPIO/32] & mask) >> (GPIO % 32); //operation to put the wanted bit on the value
	//pr_alert("%s: value read is %d\n",__FUNCTION__,value);
	return value;
}
