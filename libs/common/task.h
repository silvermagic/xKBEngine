#pragma once

#include <vector>
#include <memory>

namespace xKBEngine {

/**
    �������
 */
class Task
{
public:
	virtual ~Task() {}
	virtual bool process() = 0;
};

}