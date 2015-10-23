#pragma once
class InputEnabled {
public:
	virtual ~InputEnabled() {}
	virtual void handle_mouse_press(int button, int action, int mods, double delta_time) { return; };
	virtual void handle_mouse_entered(int entered, double delta_time) { return; }
	virtual void handle_mouse_movement(double x, double y, double delta_x, double delta_y, double acc_x, double acc_y, double delta_time) { return; }
	virtual void handle_key_inputs(int key, int scan_code, int action, int mods, double delta_time) { return; }
	virtual void handle_scroll_actions(double offset_x, double offset_y, double delta_time) { return; }
	virtual void handle_joystick(const float *axes, int axes_count, const unsigned char *state, int state_count, const char* name, double delta_time) { return; }
	virtual void handle_multiple_keystrokes(GLFWwindow *window, double delta_time) { return; }

protected:
	InputEnabled() {};
};
