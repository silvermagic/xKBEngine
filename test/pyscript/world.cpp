#include "world.h"

namespace xKBEngine
{

bool World::_World_py_installed = false;

PyMethodDef World::_World_lpScriptmethods[] = {
	{ "createCharacter", (PyCFunction)&__py_createCharacter, METH_VARARGS, 0},
	{ 0, 0, 0, 0 }
};

PyTypeObject World::_scriptType =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"World",												/* tp_name            */
	sizeof(World),											/* tp_basicsize       */
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
	"xKBEngine::World objects.",							/* tp_doc             */
	0,														/* tp_traverse        */
	0,														/* tp_clear           */
	0,														/* tp_richcompare     */
	0,														/* tp_weaklistoffset  */
	0,														/* tp_iter            */
	0,														/* tp_iternext        */
	_World_lpScriptmethods,									/* tp_methods         */
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

PyObject* World::__py_createCharacter(PyObject* self, PyObject *args)
{
	const char *name;
	int level;
	if (!PyArg_ParseTuple(args, "si", &name, &level))
	{
		BOOST_LOG_TRIVIAL(error) << "Incorrect parameter format!";
		S_Return;
	}
	PyObject *character = new Character(name, level);
	static_cast<World*>(self)->onCreateCharacter(character);
	Py_INCREF(character);
	return character;
}

World::World():
	ScriptObject(getScriptType()), 
	characters_()
{
}

World::~World()
{
	characters_.clear();
}

void World::onCreateCharacter(PyObject *obj)
{
	Py_INCREF(obj);
	characters_.push_back(obj);
}

}