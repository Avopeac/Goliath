#pragma once

class KeyAction {

public:

	enum class ActionType {
		MOVE_LEFT = 0,
		MOVE_RIGHT = 1,
		MOVE_FORWARD = 2,
		MOVE_BACK = 3,
		MOVE_UP = 4,
		MOVE_DOWN = 5,
		JUMP_UP = 6,
		NO_ACTION = -1,
		LIGHT_LEFT = 10,
		LIGHT_RIGHT = 11,
		LIGHT_FORWARD = 12,
		LIGHT_BACK = 13,
		LIGHT_UP = 14,
		LIGHT_DOWN = 15,
		INCREASE_SPEED = 16,
		DECREASE_SPEED = 17
	};

	struct Key {
		int code;
		Key() = default;
		Key(int code) { this->code = code; }
		bool operator==(const struct Key &key) const { return code == key.code; }
		bool operator<(const struct Key &key) const { return code < key.code; }
	};

	KeyAction(Key key, ActionType type) {
		this->key = key;
		this->type = type;
	}

	~KeyAction() = default;

	const Key &GetKey() const {
		return this->key; 
	};

	const ActionType &GetActionType() const {
		return this->type;
	};

private:
	Key key;
	ActionType type;

};