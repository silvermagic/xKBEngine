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
	�ͷŵ�ǰ��������Character����ֻ����C++�����д���������ʹ��delete��ʽɾ���������ǽ���Python������������
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<World*>(self);
	}

public:
	/**
	    �����󱣴浽���ݿ�
	*/
	static PyObject* __py_createCharacter(PyObject* self, PyObject *args);

public:

	static bool _World_py_installed;

	/**
	��װ��ǰ�ű�ģ��
	@param mod: ��Ҫ�������ģ��
	@param name: ������ģ����
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
	    ж�ص�ǰ�ű�ģ��
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

	/** ����ӿڿ��Ի�õ�ǰģ��Ľű����
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