#pragma once

#include <string>
#include <map>
#include <Python.h>
#include <boost/log/trivial.hpp>

namespace xKBEngine { namespace script {

// python��Ĭ�Ͽշ���ֵ
#define S_Return { Py_INCREF(Py_None); return Py_None; }	

/// �����ǰ�ű������Ĵ�����Ϣ	
#define SCRIPT_ERROR_CHECK()																\
{																							\
 	if (PyErr_Occurred())																	\
 	{																						\
		PyErr_PrintEx(0);																	\
	}																						\
}

class ScriptObject : public PyObject
{
	static PyTypeObject _scriptType;

	/** 
	    �����ȡ��ģ������Ի��߷���
	*/
	static PyObject* _tp_getattro(PyObject* self, PyObject* name)
	{
		return static_cast<ScriptObject*>(self)->onScriptGetAttro(name);
	}

	/**
	    �����ʼ����ģ�����
	*/
	static int _tp_init(PyObject* self, PyObject *args, PyObject* kwds)
	{
		return static_cast<ScriptObject*>(self)->onScriptInit(self, args, kwds);
	}

	/**
	    ��python�д����ű�ģ�����
	*/
	static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
	{
		return ScriptObject::onScriptNew(type, args, kwds);
	}

	/**
	    ��ȡ��ģ������ʾ
	*/
	static PyObject* _tp_repr(PyObject* self)
	{
		return static_cast<ScriptObject*>(self)->onScriptRepr();
	}

	/**
	    �������ñ�ģ������Ի��߷���
	*/
	static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value)
	{
		return static_cast<ScriptObject*>(self)->onScriptSetAttro(name, value);
	}

	/**
	    ��ȡ��ģ������ʾ
	*/
	static PyObject* _tp_str(PyObject* self)
	{
		return static_cast<ScriptObject*>(self)->onScriptStr();
	}

public:

	static bool _ScriptObject_py_installed;

	/**
	    ��װ��ǰ�ű�ģ��
	    @param mod: ��Ҫ�������ģ��
		@param name: ������ģ����
	*/
	static void installScript(PyObject* mod, const char* name = "ScriptObject")
	{
		ScriptObject::onInstallScript(mod);
		if (PyType_Ready(&_scriptType) < 0){
			BOOST_LOG_TRIVIAL(error) << "PyType_Ready(ScriptObject) is error!";
			PyErr_Print();
			return;
		}

		if(mod)
		{
			Py_INCREF(&_scriptType);
			if(PyModule_AddObject(mod, name, (PyObject *)&_scriptType) < 0)
			{
				BOOST_LOG_TRIVIAL(error) << "PyModule_AddObject("<< name <<") is error!";
			}
		}

		SCRIPT_ERROR_CHECK();
		_ScriptObject_py_installed = true;
		ScriptObject::scriptObjectTypes[name] = &_scriptType;
	}

	/**
	    ж�ص�ǰ�ű�ģ��
	*/
	static void uninstallScript(void)
	{
		ScriptObject::onUninstallScript();
		if (_ScriptObject_py_installed)
			Py_DECREF(&_scriptType);
	}

public:
	ScriptObject(PyTypeObject* pyType, bool isInitialised = false);
	virtual ~ScriptObject();

	/**
	    ��ȡkbe�ű����
	*/
	static PyTypeObject* getScriptObjectType(const std::string& name);

	/** ����ӿڿ��Ի�õ�ǰģ��Ľű����
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    �ű�����װʱ������
	*/
	static void onInstallScript(PyObject* mod) {}

	/**
	    �ű���ж��ʱ������
	*/
	static void onUninstallScript() {}

	/**
	    ��ȡ�����������
	*/
	const char* scriptName() const { return ob_type->tp_name; }

	//-------------------------------------------------------------------
	//                        Python��չ���ͷ���
	//-------------------------------------------------------------------

	/**
	    �ű������ȡ���Ի��߷���
	*/
	PyObject* onScriptGetAttro(PyObject* name);

	/**
	    �ű������ʼ��
	*/
	int onScriptInit(PyObject* self, PyObject *args, PyObject* kwds);

	/**
	    �ű����󴴽�һ���ö���
	*/
	static PyObject* onScriptNew(PyTypeObject* type, PyObject* args, PyObject* kwds);

	/**
	    ��ö��������
	*/
	PyObject* onScriptRepr();

	/**
	    �ű������������Ի��߷���
	*/
	int onScriptSetAttro(PyObject* attr, PyObject* value);

	/**
	    ��ö��������
	*/
	PyObject* onScriptStr();

public:
	static std::map<std::string, PyTypeObject*> scriptObjectTypes;
};

}
}

