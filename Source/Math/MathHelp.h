#pragma once
class MathHelp {
public:
	static void double_to_two_floats(double value, float &high, float &low) {
		high = (float)value;
		low = (float)(value - high);
	}
};