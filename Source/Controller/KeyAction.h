#pragma once
///In the key action class, actions are defined as enumerables.
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

	//A key is just a wrapper for some integral type that denotes a keyboard key
	struct Key {
		int _code;
		Key() = default;
		Key(int code) { _code = code; }
		bool operator==(const struct Key &key) const { return _code == key._code; }
		bool operator<(const struct Key &key) const { return _code < key._code; }
	};

	KeyAction(Key key, ActionType type) {
		_key = key;
		_type = type;
	}
	~KeyAction() = default;

	const Key &get_key() const {
		return this->_key; 
	};

	const ActionType &get_action_type() const {
		return this->_type;
	};

private:
	Key _key;
	ActionType _type;

};