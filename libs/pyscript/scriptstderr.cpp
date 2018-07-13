#include "scriptstderr.h"

namespace xKBEngine { namespace script {

bool ScriptStdErr::_ScriptStdErr_py_installed = false;

PyMethodDef ScriptStdErr::_ScriptStdErr_lpScriptmethods[] = {
	{ "write", (PyCFunction)&__py_write, METH_VARARGS, 0},
	{ "flush", (PyCFunction)&__py_flush, METH_VARARGS, 0 },
	{ 0, 0, 0, 0 }
};

PyTypeObject ScriptStdErr::_scriptType =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"ScriptStdErr",											/* tp_name            */
	sizeof(ScriptStdErr),									/* tp_basicsize       */
	0,														/* tp_itemsize        */
	(destructor)_tp_dealloc,								/* tp_dealloc         */
	0,														/* tp_print           */
	0,														/* tp_getattr         */
	0,														/* tp_setattr         */
	0,														/* tp_compare         */
	0,														/* tp_repr            */
	0,														/* tp_as_number       */
	0,														/* tp_as_sequence     */
	0,														/* tp_as_mapping      */
	0,														/* tp_hash            */
	0,														/* tp_call            */
	0,														/* tp_str             */
	0,														/* tp_getattro        */
	0,														/* tp_setattro        */
	0,														/* tp_as_buffer       */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,				/* tp_flags           */
	"xKBEngine::ScriptStdErr objects.",						/* tp_doc             */
	0,														/* tp_traverse        */
	0,														/* tp_clear           */
	0,														/* tp_richcompare     */
	0,														/* tp_weaklistoffset  */
	0,														/* tp_iter            */
	0,														/* tp_iternext        */
	_ScriptStdErr_lpScriptmethods,							/* tp_methods         */
	0,														/* tp_members         */
	0,														/* tp_getset          */
	ScriptObject::getScriptType(),							/* tp_base            */
	0,														/* tp_dict            */
	0,														/* tp_descr_get       */
	0,														/* tp_descr_set       */
	0,														/* tp_dictoffset      */
	0,														/* tp_init            */
	0,														/* tp_alloc           */
	0,														/* tp_new             */
	0,														/* tp_free            */
};

PyObject* ScriptStdErr::__py_write(PyObject* self, PyObject *args)
{
	PyObject* obj = NULL;

	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdErr::write: Bad args";
		return NULL;
	}
	
	ScriptStdErrHook *pyScriptStdErrHook = static_cast<ScriptStdErr*>(self)->pScriptStdErrHook();
	if (pyScriptStdErrHook)
	{
		Py_ssize_t size = 0;
		wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(obj, &size);
		pyScriptStdErrHook->onWrite(PyUnicode_AsWideCharStringRet0, size);
		PyMem_Free(PyUnicode_AsWideCharStringRet0);
	}
	
	S_Return;
}

PyObject* ScriptStdErr::__py_flush(PyObject* self, PyObject *args)
{
	S_Return;
}

ScriptStdErr::ScriptStdErr(ScriptStdErrHook *pScriptStdErrHook):
	ScriptObject(getScriptType()),
	old_stderr_(NULL),
	isInstall_(false),
	pScriptStdErrHook_(pScriptStdErrHook)
{
}

ScriptStdErr::~ScriptStdErr()
{
}

bool ScriptStdErr::install(void)
{
	BOOST_LOG_TRIVIAL(debug) << "ScriptStdErr::install: replace sys.stderr module.";

	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdErr: Failed to import sys module";
		return false;
	}

	old_stderr_ = PyObject_GetAttrString(sysModule, "stderr");

	PyObject_SetAttrString(sysModule, "stderr", (PyObject *)this);
	isInstall_ = true;
	Py_DECREF(sysModule);
	return true;
}

bool ScriptStdErr::uninstall(void)
{
	BOOST_LOG_TRIVIAL(debug) << "ScriptStdErr::uninstall: recovery sys.stderr module.";

	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdErr: Failed to import sys module";
		return false;
	}

	if (old_stderr_)
	{
		PyObject_SetAttrString(sysModule, "stderr", old_stderr_);
		Py_DECREF(old_stderr_);
		old_stderr_ = NULL;
	}

	Py_DECREF(sysModule);
	isInstall_ = false;
	return true;
}

}
}