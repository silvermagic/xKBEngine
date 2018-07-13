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
	    释放当前对象，由于ScriptStdErr对象只会在C++代码中创建，所以使用delete方式删除，而不是交由Python解释器来回收
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<ScriptStdErr*>(self);
	}

public:
	/**
	    Python写操作
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

public:

	static bool _ScriptStdErr_py_installed;

	/**
	    安装当前脚本模块
	    @param mod: 所要导入的主模块
	    @param name: 导入后的模块名
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
	    卸载当前脚本模块
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

	/** 这个接口可以获得当前模块的脚本类别
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    替换Python的sys模块的标准错误输出
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const { return isInstall_; }

	ScriptStdErrHook* pScriptStdErrHook() const { return pScriptStdErrHook_; }

protected:
	PyObject * old_stderr_; // 旧的sys模块的标准错误对象
	bool isInstall_;
	ScriptStdErrHook *pScriptStdErrHook_;
};

/**
    一般并不直接创建ScriptStdErr对象，而是通过ScriptStdErrHook的install方法来生成ScriptStdErr对象，因为ScriptStdErr对象主要是完成sys.stderr的替换，
	真正的重定向逻辑交由ScriptStdErrHook的onWrite方法来实现。
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
	    重定向标准错误
	*/
	bool install(void)
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdErrHook::install: hook standard error output.";

		pStderr_ = new ScriptStdErr(this);
		isInstall_ = pStderr_->install() && pStderr_->isInstall();
		return isInstall_;
	}

	/**
	    恢复标准错误
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
	    将Python标准错误写入日志或保存到缓存
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
	std::wstring wbuffer_; // 缓存Python标准错误内容
};

}
}

