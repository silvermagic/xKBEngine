#pragma once

#include <vector>
#include <memory>

namespace xKBEngine {

/**
    ÈÎÎñ³éÏó
 */
class Task
{
public:
	virtual ~Task() {}
	virtual bool process() = 0;
};

}