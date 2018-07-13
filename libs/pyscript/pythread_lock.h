#pragma once

#include <Python.h>

namespace xKBEngine { namespace script {

class PyThreadStateLock
{
public:
	PyThreadStateLock(void)
	{
		state = PyGILState_Ensure();
	}

	~PyThreadStateLock(void)
	{
		PyGILState_Release(state);
	}
private:
		PyGILState_STATE state;
};

}
}