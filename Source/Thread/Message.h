#pragma once
class Message {
public:
	virtual ~Message() {}
	virtual void process() = 0;
protected:
	Message() {}
};