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
	    释放当前对象，由于ScriptStdOut对象只会在C++代码中创建，所以使用delete方式删除，而不是交由Python解释器来回收
	*/
	static void _tp_dealloc(PyObject* self)
	{
		delete static_cast<ScriptStdOut*>(self);
	}

public:
	/**
	    Python写操作
	*/
	static PyObject* __py_write(PyObject* self, PyObject *args);
	static PyObject* __py_flush(PyObject* self, PyObject *args);

public:

	static bool _ScriptStdOut_py_installed;

	/**
	    安装当前脚本模块
	    @param mod: 所要导入的主模块
	    @param name: 导入后的模块名
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
	    卸载当前脚本模块
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

	/** 这个接口可以获得当前模块的脚本类别
	*/
	static PyTypeObject* getScriptType(void)
	{
		return &_scriptType;
	}

	/**
	    替换Python的sys模块的标准输出
	*/
	bool install(void);
	bool uninstall(void);
	bool isInstall(void) const { return isInstall_; }

	ScriptStdOutHook* pScriptStdOutHook() const { return pScriptStdOutHook_; }

protected:
	PyObject * old_stdout_; // 旧的sys模块的标准输出对象
	bool isInstall_;
	ScriptStdOutHook *pScriptStdOutHook_;
};

/**
    一般并不直接创建ScriptStdOut对象，而是通过ScriptStdOutHook的install方法来生成ScriptStdOut对象，因为ScriptStdOut对象主要是完成sys.stdout的替换，
    真正的重定向逻辑交由ScriptStdOutHook的onWrite方法来实现。
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
	    重定向标准输出
	*/
	bool install()
	{
		BOOST_LOG_TRIVIAL(debug) << "ScriptStdOutHook::install: hook standard output.";

		pStdOut_ = new ScriptStdOut(this);
		isInstall_ = pStdOut_->install() && pStdOut_->isInstall();
		return isInstall_;
	}

	/**
	    恢复标准输出
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
	    将Python标准输出写入日志或保存到缓存
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
	std::wstring wbuffer_; // 缓存Python标准输出内容
};

}
}

