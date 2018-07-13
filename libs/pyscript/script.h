#pragma once

#include <string>
#include <Python.h>
#include "common/singleton.h"
#include "scriptstderr.h"
#include "scriptstdout.h"

namespace xKBEngine { namespace script {

class Script : public Singleton<Script>
{
public:
	Script();
	~Script();

	/**
	    获取脚本扩展模块
	*/
	PyObject* getExtraModule(void) const { return extraModule_; }

	/**
	    获取脚本基础模块
	*/
	PyObject* getModule(void) const { return module_; }

	/**
	    安装脚本模块
	*/
	virtual bool install(std::wstring pythonHomeDir, std::wstring pyPaths, const char* moduleName = "KBEngine");

	/**
	    安装脚本扩展模块
	*/
	bool installExtraModule(const char* moduleName);

	/**
	    获取Python标准错误输出
	*/
	ScriptStdErrHook* pyStdErrHook() const { return pyStdErrHook_; }

	/**
	    获取Python标准错误输出
	*/
	ScriptStdOutHook* pyStdOutHook() const { return pyStdOutHook_; }

	/**
	    添加一个扩展接口到引擎扩展模块
	*/
	bool registerExtraMethod(const char* attrName, PyMethodDef* pyFunc);

	/**
	    添加一个扩展属性到引擎扩展模块
	*/
	bool registerExtraObject(const char* attrName, PyObject* pyObj);

	/**
	    添加一个扩展属性到引擎基础模块
	*/
	int registerObject(const char* attrName, PyObject* pyObj);

	/**
	    执行一段Python语句
	*/
	int run_simpleString(std::string command, std::string* retBufferPtr = NULL);

	/**
	    卸载脚本模块
	*/
	virtual bool uninstall(void);

	/**
	    从引擎基础模块删除一个扩展属性
	*/
	int unregisterObject(const char* attrName);

protected:
	PyObject*					module_;
	PyObject*					extraModule_;		// 扩展脚本模块

	ScriptStdErrHook*			pyStdErrHook_;
	ScriptStdOutHook*			pyStdOutHook_;
};

}
}