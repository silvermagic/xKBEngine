#include "character.h"

namespace xKBEngine
{

bool Character::_Character_py_installed = false;

PyMethodDef Character::_Character_lpScriptmethods[] = {
	{ "writeDB", (PyCFunction)&__py_writeDB, METH_VARARGS, 0},
	{ 0, 0, 0, 0 }
};

PyTypeObject Character::_scriptType =
{
	PyVarObject_HEAD_INIT(NULL, 0)
	"Character",											/* tp_name            */
	sizeof(Character),										/* tp_basicsize       */
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
	"xKBEngine::Character objects.",						/* tp_doc             */
	0,														/* tp_traverse        */
	0,														/* tp_clear           */
	0,														/* tp_richcompare     */
	0,														/* tp_weaklistoffset  */
	0,														/* tp_iter            */
	0,														/* tp_iternext        */
	_Character_lpScriptmethods,								/* tp_methods         */
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

PyObject* Character::__py_writeDB(PyObject* self, PyObject *args)
{
	static_cast<Character*>(self)->onWriteDB();
	S_Return;
}

Character::Character(const char * name, int level):
	ScriptObject(getScriptType()),
	name_(name),
	level_(level)
{
}

Character::~Character()
{
}

void Character::onWriteDB()
{
	BOOST_LOG_TRIVIAL(info) << "INSERT INTO Character VALUES (" << name_ << ", " << level_ << ")";
}

}