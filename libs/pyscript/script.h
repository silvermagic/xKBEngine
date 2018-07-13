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
	    ��ȡ�ű���չģ��
	*/
	PyObject* getExtraModule(void) const { return extraModule_; }

	/**
	    ��ȡ�ű�����ģ��
	*/
	PyObject* getModule(void) const { return module_; }

	/**
	    ��װ�ű�ģ��
	*/
	virtual bool install(std::wstring pythonHomeDir, std::wstring pyPaths, const char* moduleName = "KBEngine");

	/**
	    ��װ�ű���չģ��
	*/
	bool installExtraModule(const char* moduleName);

	/**
	    ��ȡPython��׼�������
	*/
	ScriptStdErrHook* pyStdErrHook() const { return pyStdErrHook_; }

	/**
	    ��ȡPython��׼�������
	*/
	ScriptStdOutHook* pyStdOutHook() const { return pyStdOutHook_; }

	/**
	    ���һ����չ�ӿڵ�������չģ��
	*/
	bool registerExtraMethod(const char* attrName, PyMethodDef* pyFunc);

	/**
	    ���һ����չ���Ե�������չģ��
	*/
	bool registerExtraObject(const char* attrName, PyObject* pyObj);

	/**
	    ���һ����չ���Ե��������ģ��
	*/
	int registerObject(const char* attrName, PyObject* pyObj);

	/**
	    ִ��һ��Python���
	*/
	int run_simpleString(std::string command, std::string* retBufferPtr = NULL);

	/**
	    ж�ؽű�ģ��
	*/
	virtual bool uninstall(void);

	/**
	    ���������ģ��ɾ��һ����չ����
	*/
	int unregisterObject(const char* attrName);

protected:
	PyObject*					module_;
	PyObject*					extraModule_;		// ��չ�ű�ģ��

	ScriptStdErrHook*			pyStdErrHook_;
	ScriptStdOutHook*			pyStdOutHook_;
};

}
}