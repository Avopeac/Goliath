#pragma once

class MathHelp {
public:
	static void doubleToTwoFloats(double value, float &high, float &low) {
		high = (float)value;
		low = (float)(value - high);
	}
};