#include "scriptobject.h"

namespace xKBEngine { namespace script {

PyTypeObject ScriptObject::_scriptType =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"ScriptObject",											/* tp_name            */
	sizeof(ScriptObject),									/* tp_basicsize       */
	0,														/* tp_itemsize        */
	0,														/* tp_dealloc         */
	0,														/* tp_print           */
	0,														/* tp_getattr         */
	0,														/* tp_setattr         */
	0,														/* tp_compare         */
	ScriptObject::_tp_repr,									/* tp_repr            */
	0,														/* tp_as_number       */
	0,														/* tp_as_sequence     */
	0,														/* tp_as_mapping      */
	0,														/* tp_hash            */
	0,														/* tp_call            */
	ScriptObject::_tp_str,									/* tp_str             */
	(getattrofunc)ScriptObject::_tp_getattro,				/* tp_getattro        */
	(setattrofunc)ScriptObject::_tp_setattro,				/* tp_setattro        */
	0,														/* tp_as_buffer       */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,				/* tp_flags           */
	"xKBEngine::ScriptObject objects.",						/* tp_doc             */
	0,														/* tp_traverse        */
	0,														/* tp_clear           */
	0,														/* tp_richcompare     */
	0,														/* tp_weaklistoffset  */
	0,														/* tp_iter            */
	0,														/* tp_iternext        */
	0,														/* tp_methods         */
	0,														/* tp_members         */
	0,														/* tp_getset          */
	0,														/* tp_base            */
	0,														/* tp_dict            */
	0,														/* tp_descr_get       */
	0,														/* tp_descr_set       */
	0,														/* tp_dictoffset      */
	0,														/* tp_init            */
	0,														/* tp_alloc           */
	ScriptObject::_tp_new,									/* tp_new             */
	0,														/* tp_free            */
};

std::map<std::string, PyTypeObject*> ScriptObject::scriptObjectTypes;

ScriptObject::ScriptObject(PyTypeObject* pyType, bool isInitialised)
{
	if (PyType_Ready(pyType) < 0)
	{
		BOOST_LOG_TRIVIAL(error) << "ScriptObject: Type " << pyType->tp_name  << " is not ready";
	}

	if (!isInitialised)
	{
		PyObject_INIT(static_cast<PyObject*>(this), pyType);
	}
}


ScriptObject::~ScriptObject()
{
}

PyTypeObject* ScriptObject::getScriptObjectType(const std::string& name)
{
	auto iter = scriptObjectTypes.find(name);
	if (iter != scriptObjectTypes.end())
		return iter->second;

	return NULL;
}

PyObject* ScriptObject::onScriptGetAttro(PyObject* name)
{
	return PyObject_GenericGetAttr(this, name);
}

int ScriptObject::onScriptInit(PyObject* self, PyObject *args, PyObject* kwds)
{
	return 0;
}

PyObject* ScriptObject::onScriptNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
	return type->tp_alloc(type, 0);
}

PyObject* ScriptObject::onScriptRepr()
{
	return PyUnicode_FromFormat("%s object at %p.", this->scriptName(), this);
}

int ScriptObject::onScriptSetAttro(PyObject* attr, PyObject* value)
{
	return PyObject_GenericSetAttr(static_cast<PyObject*>(this), attr, value);
}

PyObject* ScriptObject::onScriptStr()
{
	return PyUnicode_FromFormat("%s object at %p.", this->scriptName(), this);
}

}
}