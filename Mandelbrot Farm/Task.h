#pragma once

class Task
{
public:
	virtual ~Task() {}

	virtual void run() = 0;
};
