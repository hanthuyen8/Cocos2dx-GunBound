#pragma once

#include <string>

class ISubscriber
{
public:
	virtual ~ISubscriber() {};
	virtual void onNotify(const std::string& message) = 0;
};

class IPublisher
{
public:
	virtual ~IPublisher() {};
	virtual void subcribe(ISubscriber* member) = 0;
	virtual void unsubcribe(ISubscriber* member) = 0;
	virtual void notify() = 0;
};

