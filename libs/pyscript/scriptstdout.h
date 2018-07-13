#pragma once

#include <locale>
#include <boost/locale/encoding_utf.hpp>
#include "scriptobject.h"

namespace xKBEngine { namespace script {

class ScriptStdOutHook;
class ScriptStdOut : public ScriptObject
{
	static PyTypeObject _scriptType;
	static PyMethodDef _ScriptStdOut_lpScriptmethods[];

	/**
	    �ͷŵ�ǰ��������ScriptStdOut����ֻ����C++�����д���������ʹ��delete��ʽɾ���������ǽ���Python������������
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<ScriptStdOut*>(self);
	}

public:
	/**
	    Pythonд����
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

public:

	static bool _ScriptStdOut_py_installed;

	/**
	    ��װ��ǰ�ű�ģ��
	    @param mod: ��Ҫ�������ģ��
	    @param name: ������ģ����
	*/
	static void installScript(PyObject* mod, const char* name = "ScriptStdOut")
	{
		ScriptObject::onInstallScript(mod);
		if (PyType_Ready(&_scriptType) < 0) {
			BOOST_LOG_TRIVIAL(error) << "PyType_Ready(ScriptStdOut) is error!";
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
		_ScriptStdOut_py_installed = true;
		ScriptObject::scriptObjectTypes[name] = &_scriptType;
	}

	/**
	    ж�ص�ǰ�ű�ģ��
	*/
	static void uninstallScript(void)
	{
		ScriptStdOut::onUninstallScript();
		if (_ScriptStdOut_py_installed)
			Py_DECREF(&_scriptType);
	}

public:
	ScriptStdOut(ScriptStdOutHook *pScriptStdOutHook);
	~ScriptStdOut();

	/** ����ӿڿ��Ի�õ�ǰģ��Ľű����
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    �滻Python��sysģ��ı�׼���
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const { return isInstall_; }

	ScriptStdOutHook* pScriptStdOutHook() const { return pScriptStdOutHook_; }

protected:
	PyObject * old_stdout_; // �ɵ�sysģ��ı�׼�������
	bool isInstall_;
	ScriptStdOutHook *pScriptStdOutHook_;
};

/**
    һ�㲢��ֱ�Ӵ���ScriptStdOut���󣬶���ͨ��ScriptStdOutHook��install����������ScriptStdOut������ΪScriptStdOut������Ҫ�����sys.stdout���滻��
    �������ض����߼�����ScriptStdOutHook��onWrite������ʵ�֡�
*/

class ScriptStdOutHook
{
public:
	ScriptStdOutHook(std::string* buffer = NULL):
		pStdOut_(NULL),
		isInstall_(false),
		buffer_(buffer),
		wbuffer_()
	{
	}

	virtual ~ScriptStdOutHook() 
	{ 
		if (isInstall_)
			uninstall();
	}

	/**
	    �ض����׼���
	*/
	bool install()
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdOutHook::install: hook standard output.";

		pStdOut_ = new ScriptStdOut(this);
		isInstall_ = pStdOut_->install() && pStdOut_->isInstall();
		return isInstall_;
	}

	/**
	    �ָ���׼���
	*/
	bool uninstall(void)
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdOutHook::uninstall: unhook standard output.";

		if (pStdOut_)
		{
			if (!pStdOut_->uninstall())
			{
				Py_DECREF(pStdOut_);
				return false;
			}

			Py_DECREF(pStdOut_);
			pStdOut_ = NULL;
		}

		isInstall_ = false;
		return true;
	}

	/**
	    ��Python��׼���д����־�򱣴浽����
	*/
	virtual void onWrite(const wchar_t* msg, uint32_t msglen)
	{
		std::wstring str;
		str.assign(msg, msglen);
		wbuffer_ += str;

		if (msg[0] == L'\n')
		{
			std::string out = boost::locale::conv::utf_to_utf<char>(wbuffer_);
			BOOST_LOG_TRIVIAL(info) << out;
			if (buffer_)
			{
				(*buffer_) += out;
			}
			wbuffer_ = L"";
		}
	}

protected:
	ScriptStdOut* pStdOut_;
	bool isInstall_;
	std::string* buffer_;
	std::wstring wbuffer_; // ����Python��׼�������
};

}
}

