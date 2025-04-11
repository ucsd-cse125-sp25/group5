#include "DOF.h"


DOF::DOF() {
	min = -100000; 
	max =  100000;
	value = 0;
}
void DOF::SetValue(float val) {
	value = val;
}

double DOF::GetValue() {
	if (value > max) return max;
	if (value < min) return min;
	return value;
}

void DOF::SetMinMax(float new_min, float new_max) {
	min = new_min;
	max = new_max;
}