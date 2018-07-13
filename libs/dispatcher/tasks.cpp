#include <algorithm>
#include "common/task.h"
#include "tasks.h"

namespace xKBEngine { namespace dispatcher {

Tasks::Tasks():
	container_()
{
}

Tasks::~Tasks()
{
	container_.clear();
}

void Tasks::add(std::shared_ptr<Task> pTask )
{
	container_.push_back(pTask);
}

bool Tasks::cancel(std::shared_ptr<Task> pTask )
{
	auto iter = std::find(container_.begin(), container_.end(), pTask);
	if (iter != container_.end())
	{
		container_.erase( iter );
		return true;
	}

	return false;
}

void Tasks::process()
{
	auto iter = container_.begin();

	while (iter != container_.end())
	{
		std::shared_ptr<Task> pTask = *iter;
		if(!pTask->process())
			iter = container_.erase(iter);
		else
			++iter;
	}
}

}
}