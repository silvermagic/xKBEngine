#pragma once

#include <string>
#include <map>
#include <Python.h>
#include <boost/log/trivial.hpp>

namespace xKBEngine { namespace script {

// python的默认空返回值
#define S_Return { Py_INCREF(Py_None); return Py_None; }	

/// 输出当前脚本产生的错误信息	
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
	    请求获取本模块的属性或者方法
	*/
	static PyObject* _tp_getattro(PyObject* self, PyObject* name)
	{
		return static_cast<ScriptObject*>(self)->onScriptGetAttro(name);
	}

	/**
	    请求初始化本模块对象
	*/
	static int _tp_init(PyObject* self, PyObject *args, PyObject* kwds)
	{
		return static_cast<ScriptObject*>(self)->onScriptInit(self, args, kwds);
	}

	/**
	    从python中创建脚本模块对象
	*/
	static PyObject* _tp_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
	{
		return ScriptObject::onScriptNew(type, args, kwds);
	}

	/**
	    获取本模块对象表示
	*/
	static PyObject* _tp_repr(PyObject* self)
	{
		return static_cast<ScriptObject*>(self)->onScriptRepr();
	}

	/**
	    请求设置本模块的属性或者方法
	*/
	static int _tp_setattro(PyObject* self, PyObject* name, PyObject* value)
	{
		return static_cast<ScriptObject*>(self)->onScriptSetAttro(name, value);
	}

	/**
	    获取本模块对象表示
	*/
	static PyObject* _tp_str(PyObject* self)
	{
		return static_cast<ScriptObject*>(self)->onScriptStr();
	}

public:

	static bool _ScriptObject_py_installed;

	/**
	    安装当前脚本模块
	    @param mod: 所要导入的主模块
		@param name: 导入后的模块名
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
	    卸载当前脚本模块
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
	    获取kbe脚本类别
	*/
	static PyTypeObject* getScriptObjectType(const std::string& name);

	/** 这个接口可以获得当前模块的脚本类别
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    脚本被安装时被调用
	*/
	static void onInstallScript(PyObject* mod) {}

	/**
	    脚本被卸载时被调用
	*/
	static void onUninstallScript() {}

	/**
	    获取对象类别名称
	*/
	const char* scriptName() const { return ob_type->tp_name; }

	//-------------------------------------------------------------------
	//                        Python扩展类型方法
	//-------------------------------------------------------------------

	/**
	    脚本请求获取属性或者方法
	*/
	PyObject* onScriptGetAttro(PyObject* name);

	/**
	    脚本请求初始化
	*/
	int onScriptInit(PyObject* self, PyObject *args, PyObject* kwds);

	/**
	    脚本请求创建一个该对象
	*/
	static PyObject* onScriptNew(PyTypeObject* type, PyObject* args, PyObject* kwds);

	/**
	    获得对象的描述
	*/
	PyObject* onScriptRepr();

	/**
	    脚本请求设置属性或者方法
	*/
	int onScriptSetAttro(PyObject* attr, PyObject* value);

	/**
	    获得对象的描述
	*/
	PyObject* onScriptStr();

public:
	static std::map<std::string, PyTypeObject*> scriptObjectTypes;
};

}
}

