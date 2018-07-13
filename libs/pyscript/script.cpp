#include "script.h"

namespace xKBEngine { 

KBE_SINGLETON_INIT(script::Script);

namespace script {

Script::Script():
	module_(NULL),
	extraModule_(NULL),
	pyStdErrHook_(NULL),
	pyStdOutHook_(NULL)
{
}


Script::~Script()
{
}

bool Script::install(std::wstring pythonHomeDir, std::wstring pyPaths, const char* moduleName)
{
	// ����Python�Ļ�������
	Py_SetPythonHome(const_cast<wchar_t*>(pythonHomeDir.c_str()));
	Py_SetPath(pyPaths.c_str());

	// Python��������ʼ��
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		BOOST_LOG_TRIVIAL(error) << "Script::install(): Py_Initialize is failed!";
		return false;
	}

	PyObject *m = PyImport_AddModule("__main__");
	// ���һ���ű�����ģ��
	module_ = PyImport_AddModule(moduleName);
	if (module_ == NULL)
		return false;

	// ��װpy�ض���ģ��
	ScriptStdOut::installScript(NULL);
	ScriptStdErr::installScript(NULL);

	// ������ģ��������__main__
	PyObject_SetAttrString(m, moduleName, module_);
	PyObject_SetAttrString(module_, "__doc__", PyUnicode_FromString("This module is created by KBEngine!"));

	// Ĭ��Hook�����ǽ�Python��ӡ�������־
	pyStdErrHook_ = new ScriptStdErrHook();
	pyStdOutHook_ = new ScriptStdOutHook();
	if (!pyStdErrHook_->install() || !pyStdOutHook_->install())
	{
		BOOST_LOG_TRIVIAL(error) << "Script::install::pyStdErrHook_->install() or Script::install::pyStdOutHook_->install() is failed!";
		delete pyStdErrHook_;
		pyStdErrHook_ = NULL;
		delete pyStdOutHook_;
		pyStdOutHook_ = NULL;
		SCRIPT_ERROR_CHECK();
		return false;
	}

	BOOST_LOG_TRIVIAL(info) << "Script::install() : is successfully, Python = (" << Py_GetVersion() << ")!";
	return installExtraModule("KBExtra");
}

bool Script::installExtraModule(const char* moduleName)
{
	PyObject *m = PyImport_AddModule("__main__");

	// ���һ���ű���չģ��
	extraModule_ = PyImport_AddModule(moduleName);
	if (extraModule_ == NULL)
		return false;

	// ��ʼ����չģ��
	PyObject *module = PyImport_AddModule(moduleName);
	if (module == NULL)
		return false;

	// ����չģ��������__main__
	PyObject_SetAttrString(m, moduleName, extraModule_);

	BOOST_LOG_TRIVIAL(info) << "Script::install(): " << moduleName <<" is successfully!";
	return true;
}

bool Script::registerExtraMethod(const char* attrName, PyMethodDef* pyFunc)
{
	return PyModule_AddObject(extraModule_, attrName, PyCFunction_New(pyFunc, NULL)) != -1;
}


bool Script::registerExtraObject(const char* attrName, PyObject* pyObj)
{
	return PyObject_SetAttrString(extraModule_, attrName, pyObj) != -1;
}


int Script::registerObject(const char* attrName, PyObject* pyObj)
{
	return PyObject_SetAttrString(module_, attrName, pyObj);
}

int Script::run_simpleString(std::string command, std::string* retBufferPtr)
{
	if (command.empty())
	{
		BOOST_LOG_TRIVIAL(error) << "Script::Run_SimpleString: command is NULL!";
		return 0;
	}

	if (retBufferPtr != NULL)
	{
		// ��Buffer��Hook�����ǽ�Python��ӡ�������־��ͬʱ���浽Buffer��
		std::shared_ptr<ScriptStdErrHook> pyStdErrHook = std::make_shared<ScriptStdErrHook>(retBufferPtr);
		std::shared_ptr<ScriptStdOutHook> pyStdOutHook = std::make_shared<ScriptStdOutHook>(retBufferPtr);
		if (!pyStdErrHook->install() && !pyStdOutHook->install())
		{
			BOOST_LOG_TRIVIAL(error) << "Script::Run_SimpleString: pyStdErrHook_->install() or pyStdOutHook_->install() is failed!";
			SCRIPT_ERROR_CHECK();
			return -1;
		}

		PyObject *m, *d, *v;
		m = PyImport_AddModule("__main__");
		if (m == NULL)
		{
			SCRIPT_ERROR_CHECK();
			return -1;
		}

		d = PyModule_GetDict(m);

		v = PyRun_StringFlags(command.c_str(), Py_single_input, d, d, NULL);
		if (v == NULL)
		{
			PyErr_Print();
			return -1;
		}

		Py_DECREF(v);
	}
	else
	{
		PyRun_SimpleStringFlags(command.c_str(), NULL);
	}

	SCRIPT_ERROR_CHECK();
	return 0;


}

bool Script::uninstall(void)
{
	if (pyStdErrHook_)
	{
		delete pyStdErrHook_;
		pyStdErrHook_ = NULL;
	}

	if (pyStdOutHook_)
	{
		delete pyStdOutHook_;
		pyStdOutHook_ = NULL;
	}

	ScriptStdOut::uninstallScript();
	ScriptStdErr::uninstallScript();

	// ж��python������
	Py_Finalize();

	BOOST_LOG_TRIVIAL(info) << "Script::uninstall(): is successfully!";
	return true;
}


int Script::unregisterObject(const char* attrName)
{
	if (module_ == NULL || attrName == NULL)
		return 0;

	return PyObject_DelAttrString(module_, attrName);
}

}
}