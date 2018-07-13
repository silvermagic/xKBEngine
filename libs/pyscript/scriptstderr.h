#pragma once

#include <locale>
#include <boost/locale/encoding_utf.hpp>
#include "scriptobject.h"

namespace xKBEngine { namespace script {

class ScriptStdErrHook;
class ScriptStdErr : public ScriptObject
{
	static PyTypeObject _scriptType;
	static PyMethodDef _ScriptStdErr_lpScriptmethods[];

	/**
	    �ͷŵ�ǰ��������ScriptStdErr����ֻ����C++�����д���������ʹ��delete��ʽɾ���������ǽ���Python������������
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<ScriptStdErr*>(self);
	}

public:
	/**
	    Pythonд����
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

public:

	static bool _ScriptStdErr_py_installed;

	/**
	    ��װ��ǰ�ű�ģ��
	    @param mod: ��Ҫ�������ģ��
	    @param name: ������ģ����
	*/
	static void installScript(PyObject* mod, const char* name = "ScriptStdErr")
	{
		ScriptObject::onInstallScript(mod);
		if (PyType_Ready(&_scriptType) < 0) {
			BOOST_LOG_TRIVIAL(error) << "PyType_Ready(ScriptStdErr) is error!";
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
		_ScriptStdErr_py_installed = true;
		ScriptObject::scriptObjectTypes[name] = &_scriptType;
	}

	/**
	    ж�ص�ǰ�ű�ģ��
	*/
	static void uninstallScript(void)
	{
		ScriptStdErr::onUninstallScript();
		if (_ScriptStdErr_py_installed)
			Py_DECREF(&_scriptType);
	}

public:
	ScriptStdErr(ScriptStdErrHook *pScriptStdErrHook);
	virtual ~ScriptStdErr();

	/** ����ӿڿ��Ի�õ�ǰģ��Ľű����
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    �滻Python��sysģ��ı�׼�������
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const { return isInstall_; }

	ScriptStdErrHook* pScriptStdErrHook() const { return pScriptStdErrHook_; }

protected:
	PyObject * old_stderr_; // �ɵ�sysģ��ı�׼�������
	bool isInstall_;
	ScriptStdErrHook *pScriptStdErrHook_;
};

/**
    һ�㲢��ֱ�Ӵ���ScriptStdErr���󣬶���ͨ��ScriptStdErrHook��install����������ScriptStdErr������ΪScriptStdErr������Ҫ�����sys.stderr���滻��
	�������ض����߼�����ScriptStdErrHook��onWrite������ʵ�֡�
*/

class ScriptStdErrHook
{
public:
	ScriptStdErrHook(std::string* buffer = NULL):
		pStderr_(NULL),
		isInstall_(false),
		buffer_(buffer),
		wbuffer_()
	{
	}

	virtual ~ScriptStdErrHook()
	{
		if (isInstall_)
			uninstall();
	}

	/**
	    �ض����׼����
	*/
	bool install(void)
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdErrHook::install: hook standard error output.";

		pStderr_ = new ScriptStdErr(this);
		isInstall_ = pStderr_->install() && pStderr_->isInstall();
		return isInstall_;
	}

	/**
	    �ָ���׼����
	*/
	bool uninstall(void)
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdErrHook::uninstall: unhook standard error output.";

		if (pStderr_)
		{
			if (!pStderr_->uninstall())
			{
				Py_DECREF(pStderr_);
				return false;
			}

			Py_DECREF(pStderr_);
			pStderr_ = NULL;
		}

		isInstall_ = false;
		return true;
	}

	bool isInstall(void) const { return isInstall_; }

	/**
	    ��Python��׼����д����־�򱣴浽����
	*/
	virtual void onWrite(const wchar_t* msg, uint32_t msglen)
	{
		std::wstring str;
		str.assign(msg, msglen);
		wbuffer_ += str;

		if (msg[0] == L'\n')
		{
			std::string out = boost::locale::conv::utf_to_utf<char>(wbuffer_);
			BOOST_LOG_TRIVIAL(error) << out;
			if (buffer_)
			{
				(*buffer_) += out;
			}
			wbuffer_ = L"";
		}
	}

protected:
	ScriptStdErr * pStderr_;
	bool isInstall_;
	std::string* buffer_;
	std::wstring wbuffer_; // ����Python��׼��������
};

}
}

