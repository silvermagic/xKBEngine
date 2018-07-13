#include <boost/log/trivial.hpp>
#include "pyscript/script.h"
#include "world.h"

using namespace xKBEngine;
using namespace xKBEngine::script;

#define S_RELEASE(pyObj)	\
	if(pyObj){				\
		Py_DECREF(pyObj);	\
		pyObj = NULL;		\
	}						\

int main()
{
	Script s;
	s.install(L"/usr/bin", L"/usr/lib64/python3.4/:../");
	Character::installScript(s.getModule());
	World::installScript(s.getModule());
	World *w = new World();
	if (!s.registerObject("world", w))
	{
		BOOST_LOG_TRIVIAL(error) << "register world object failed!";
	}

	PyObject *pyEntryScriptFileName = PyUnicode_FromString("kbemain");
	PyObject *entryScript = PyImport_Import(pyEntryScriptFileName);
	S_RELEASE(pyEntryScriptFileName);

	if (PyErr_Occurred())
	{
		BOOST_LOG_TRIVIAL(error) << "Importing scripts kbemain.py failed!";
		PyErr_PrintEx(0);
	}
	else
	{
		PyObject *func = PyObject_GetAttrString(entryScript, "main");
		if (!PyCallable_Check(func)) {
			BOOST_LOG_TRIVIAL(error) << "kbemain.main() not callable!";
			PyErr_PrintEx(0);
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) << "call kbemain.main().";
			PyEval_CallObject(func, NULL);
			S_RELEASE(func);
		}
	}
	S_RELEASE(entryScript);

	/*s.run_simpleString("import kbemain\n"
		"kbemain.main()");*/

	/*s.run_simpleString("import KBEngine\n"
		"print(dir(KBEngine))");*/

	s.unregisterObject("world");
	Py_DECREF(w);
	Character::uninstallScript();
	World::uninstallScript();
	s.uninstall();
}