#include "scriptstdout.h"

namespace xKBEngine { namespace script {

bool ScriptStdOut::_ScriptStdOut_py_installed = false;

PyMethodDef ScriptStdOut::_ScriptStdOut_lpScriptmethods[] = {
	{ "write", (PyCFunction)&__py_write, METH_VARARGS, 0},
	{ "flush", (PyCFunction)&__py_flush, METH_VARARGS, 0 },
	{ 0, 0, 0, 0 }
};

PyTypeObject ScriptStdOut::_scriptType =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"ScriptStdOut",											/* tp_name            */
	sizeof(ScriptStdOut),									/* tp_basicsize       */
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
	"xKBEngine::ScriptStdOut objects.",						/* tp_doc             */
	0,														/* tp_traverse        */
	0,														/* tp_clear           */
	0,														/* tp_richcompare     */
	0,														/* tp_weaklistoffset  */
	0,														/* tp_iter            */
	0,														/* tp_iternext        */
	_ScriptStdOut_lpScriptmethods,							/* tp_methods         */
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

PyObject* ScriptStdOut::__py_write(PyObject* self, PyObject *args)
{
	PyObject* obj = NULL;

	if (!PyArg_ParseTuple(args, "O", &obj))
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdOut::write: Bad args";
		return NULL;
	}
	
	ScriptStdOutHook *pyScriptStdOutHook = static_cast<ScriptStdOut*>(self)->pScriptStdOutHook();
	if (pyScriptStdOutHook)
	{
		Py_ssize_t size = 0;
		wchar_t* PyUnicode_AsWideCharStringRet0 = PyUnicode_AsWideCharString(obj, &size);
		pyScriptStdOutHook->onWrite(PyUnicode_AsWideCharStringRet0, size);
		PyMem_Free(PyUnicode_AsWideCharStringRet0);
	}
	
	S_Return;
}

PyObject* ScriptStdOut::__py_flush(PyObject* self, PyObject *args)
{
	S_Return;
}

ScriptStdOut::ScriptStdOut(ScriptStdOutHook *pScriptStdOutHook):
	ScriptObject(getScriptType()),
	old_stdout_(NULL),
	isInstall_(false),
	pScriptStdOutHook_(pScriptStdOutHook)
{
}

ScriptStdOut::~ScriptStdOut()
{
}

bool ScriptStdOut::install(void)
{
	BOOST_LOG_TRIVIAL(debug) << "ScriptStdOut::install: replace sys.stdout module.";

	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdOut: Failed to import sys module";
		return false;
	}

	old_stdout_ = PyObject_GetAttrString(sysModule, "stdout");

	PyObject_SetAttrString(sysModule, "stdout", (PyObject *)this);
	isInstall_ = true;
	Py_DECREF(sysModule);
	return true;
}

bool ScriptStdOut::uninstall(void)
{
	BOOST_LOG_TRIVIAL(debug) << "ScriptStdOut::uninstall: recovery sys.stdout module.";

	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptStdOut: Failed to import sys module";
		return false;
	}

	if (old_stdout_)
	{
		PyObject_SetAttrString(sysModule, "stdout", old_stdout_);
		Py_DECREF(old_stdout_);
		old_stdout_ = NULL;
	}

	Py_DECREF(sysModule);
	isInstall_ = false;
	return true;
}

}
}