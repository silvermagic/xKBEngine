#include <vector>
#include <boost/log/trivial.hpp>
#include "pyscript/scriptobject.h"
#include "character.h"

namespace xKBEngine
{

namespace script
{
	class ScriptObject;
};

class World : public script::ScriptObject
{
	static PyTypeObject _scriptType;
	static PyMethodDef _World_lpScriptmethods[];

	/**
	释放当前对象，由于Character对象只会在C++代码中创建，所以使用delete方式删除，而不是交由Python解释器来回收
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<World*>(self);
	}

public:
	/**
	    将对象保存到数据库
	*/
	static PyObject* __py_createCharacter(PyObject* self, PyObject *args);

public:

	static bool _World_py_installed;

	/**
	安装当前脚本模块
	@param mod: 所要导入的主模块
	@param name: 导入后的模块名
	*/
	static void installScript(PyObject* mod, const char* name = "World")
	{
		ScriptObject::onInstallScript(mod);
		if (PyType_Ready(&_scriptType) < 0) {
			BOOST_LOG_TRIVIAL(error) << "PyType_Ready(World) is error!";
			PyErr_Print();
			return;
		}

		if (mod)
		{
			Py_INCREF(&_scriptType);
			if (PyModule_AddObject(mod, name, (PyObject *)&_scriptType) < 0)
			{
				BOOST_LOG_TRIVIAL(error) << "PyModule_AddObject(" << name << ") is error!";
			}
		}

		SCRIPT_ERROR_CHECK();
		_World_py_installed = true;
		ScriptObject::scriptObjectTypes[name] = &_scriptType;
	}

	/**
	    卸载当前脚本模块
	*/
	static void uninstallScript(void)
	{
		Character::onUninstallScript();
		if (_World_py_installed)
			Py_DECREF(&_scriptType);
	}

public:
	World();
	virtual ~World();

	/** 这个接口可以获得当前模块的脚本类别
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	void onCreateCharacter(PyObject *obj);

protected:
	std::vector<PyObject*> characters_;
};

}