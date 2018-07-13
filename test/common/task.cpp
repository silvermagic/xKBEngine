#include <iostream>
#include <cstring>
#include "common/task.h"

using namespace xKBEngine;

class MyTask : public Task
{
public:
	MyTask(std::string name) :
		name_(name)
	{
	}

	virtual bool process()
	{
		std::cout << name_ << " process the task." << std::endl;
		return true;
	}

protected:
	std::string name_;
};

int main()
{
	// Task Test
	MyTask t("fwd");
	t.process();
    return 0;
}