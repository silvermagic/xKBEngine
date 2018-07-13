#pragma once

#include <vector>
#include <memory>

namespace xKBEngine {

class Task;

namespace dispatcher {

/**
    任务管理器
*/
class Tasks
{
public:
	Tasks();
	~Tasks();

	void add(std::shared_ptr<Task> pTask);
	bool cancel(std::shared_ptr<Task> pTask);
	void process();

protected:
	std::vector<std::shared_ptr<Task>> container_;
};

}
}