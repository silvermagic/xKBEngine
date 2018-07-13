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
	// 设置Python的环境变量
	Py_SetPythonHome(const_cast<wchar_t*>(pythonHomeDir.c_str()));
	Py_SetPath(pyPaths.c_str());

	// Python解释器初始化
	Py_Initialize();
	if (!Py_IsInitialized())
	{
		BOOST_LOG_TRIVIAL(error) << "Script::install(): Py_Initialize is failed!";
		return false;
	}

	PyObject *m = PyImport_AddModule("__main__");
	// 添加一个脚本基础模块
	module_ = PyImport_AddModule(moduleName);
	if (module_ == NULL)
		return false;

	// 安装py重定向模块
	ScriptStdOut::installScript(NULL);
	ScriptStdErr::installScript(NULL);

	// 将基础模块对象加入__main__
	PyObject_SetAttrString(m, moduleName, module_);
	PyObject_SetAttrString(module_, "__doc__", PyUnicode_FromString("This module is created by KBEngine!"));

	// 默认Hook动作是将Python打印输出到日志
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

	// 添加一个脚本扩展模块
	extraModule_ = PyImport_AddModule(moduleName);
	if (extraModule_ == NULL)
		return false;

	// 初始化扩展模块
	PyObject *module = PyImport_AddModule(moduleName);
	if (module == NULL)
		return false;

	// 将扩展模块对象加入__main__
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
		// 带Buffer的Hook动作是将Python打印输出到日志的同时保存到Buffer中
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

	// 卸载python解释器
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