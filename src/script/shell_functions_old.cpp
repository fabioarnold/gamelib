// logic
Shell::Atom shellTrue(Shell::Atom atoms[]);
Shell::Atom shellFalse(Shell::Atom atoms[]);
Shell::Atom shellNot(Shell::Atom atoms[]);
Shell::Atom shellAnd(Shell::Atom atoms[]);
Shell::Atom shellOr(Shell::Atom atoms[]);

// comparison
Shell::Atom equal(Shell::Atom atoms[]);
Shell::Atom less(Shell::Atom atoms[]);
Shell::Atom greater(Shell::Atom atoms[]);

// arithmetic
Shell::Atom add(Shell::Atom atoms[]);
Shell::Atom substract(Shell::Atom atoms[]);
Shell::Atom multiply(Shell::Atom atoms[]);
Shell::Atom divide(Shell::Atom atoms[]);
Shell::Atom modulo(Shell::Atom atoms[]);

// casting
Shell::Atom toBoolean(Shell::Atom atoms[]);
Shell::Atom toInteger(Shell::Atom atoms[]);
Shell::Atom toFloat(Shell::Atom atoms[]);
Shell::Atom toString(Shell::Atom atoms[]);

// string functions
Shell::Atom concatenate(Shell::Atom atoms[]);

// system
Shell::Atom print(Shell::Atom atoms[]);

// MY_FDF: return type, 4 * argument type
Shell::BuiltinFunction Shell::_builtinFunctions[] = {
	{"true",    {0, &shellTrue,  MY_FDF(FlagBool, 0, 0, 0, 0)}},
	{"false",   {0, &shellFalse, MY_FDF(FlagBool, 0, 0, 0, 0)}},
	{"not",     {1, &shellNot,   MY_FDF(FlagBool, FlagBool, 0, 0, 0)}},
	{"and",     {2, &shellAnd,   MY_FDF(FlagBool, FlagBool, FlagBool, 0, 0)}},
	{"or",      {2, &shellOr,    MY_FDF(FlagBool, FlagBool, FlagBool, 0, 0)}},

	{"equal",   {2, &equal,   MY_FDF(FlagBool, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"less",    {2, &less,    MY_FDF(FlagBool, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"greater", {2, &greater, MY_FDF(FlagBool, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},

	{"add",     {2, &add,       MY_FDF(FlagInt | FlagFloat, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"sub",     {2, &substract, MY_FDF(FlagInt | FlagFloat, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"mult",    {2, &multiply,  MY_FDF(FlagInt | FlagFloat, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"div",     {2, &divide,    MY_FDF(FlagInt | FlagFloat, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},
	{"mod",     {2, &modulo,    MY_FDF(FlagInt | FlagFloat, FlagInt | FlagFloat, FlagInt | FlagFloat, 0, 0)}},

	{"bool",    {1, &toBoolean, MY_FDF(FlagBool, FlagInt, 0, 0, 0)}},
	{"int",     {1, &toInteger, MY_FDF(FlagInt, FlagFloat, 0, 0, 0)}},
	{"float",   {1, &toFloat,   MY_FDF(FlagFloat, FlagInt, 0, 0, 0)}},
	{"string",  {1, &toString,  MY_FDF(FlagString, FlagBool | FlagInt | FlagFloat, 0, 0, 0)}},

	{"concat",  {2, &concatenate, MY_FDF(FlagString, FlagString, FlagString, 0, 0)}},

	{"print",   {1, &print, MY_FDF(FlagVoid, FlagString, 0, 0, 0)}}
};

Shell::Atom shellTrue(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = true;

	return result;
}

Shell::Atom shellFalse(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = false;

	return result;
}

Shell::Atom shellNot(Shell::Atom atoms[]) {
	assert(atoms[0].type == Shell::AtomBoolean);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = !atoms[0].value_bool;

	return result;
}

Shell::Atom shellAnd(Shell::Atom atoms[]) {
	assert(atoms[0].type == Shell::AtomBoolean && atoms[1].type == Shell::AtomBoolean);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = atoms[0].value_bool && atoms[1].value_bool;

	return result;
}

Shell::Atom shellOr(Shell::Atom atoms[]) {
	assert(atoms[0].type == Shell::AtomBoolean && atoms[1].type == Shell::AtomBoolean);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = atoms[0].value_bool && atoms[1].value_bool;

	return result;
}

Shell::Atom equal(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	if (atoms[0].type == Shell::AtomInteger) {
		result.value_bool = atoms[0].value_int == atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.value_bool = atoms[0].value_float == atoms[1].value_float;
	}

	return result;
}

Shell::Atom less(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	if (atoms[0].type == Shell::AtomInteger) {
		result.value_bool = atoms[0].value_int < atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.value_bool = atoms[0].value_float < atoms[1].value_float;
	}

	return result;
}

Shell::Atom greater(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	if (atoms[0].type == Shell::AtomInteger) {
		result.value_bool = atoms[0].value_int > atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.value_bool = atoms[0].value_float > atoms[1].value_float;
	}

	return result;
}

Shell::Atom add(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	if (atoms[0].type == Shell::AtomInteger) {
		result.type = Shell::AtomInteger;
		result.value_int = atoms[0].value_int + atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.type = Shell::AtomFloat;
		result.value_float = atoms[0].value_float + atoms[1].value_float;
	}

	return result;
}

Shell::Atom substract(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	if (atoms[0].type == Shell::AtomInteger) {
		result.type = Shell::AtomInteger;
		result.value_int = atoms[0].value_int - atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.type = Shell::AtomFloat;
		result.value_float = atoms[0].value_float - atoms[1].value_float;
	}

	return result;
}

Shell::Atom multiply(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	if (atoms[0].type == Shell::AtomInteger) {
		result.type = Shell::AtomInteger;
		result.value_int = atoms[0].value_int * atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.type = Shell::AtomFloat;
		result.value_float = atoms[0].value_float * atoms[1].value_float;
	}

	return result;
}

Shell::Atom divide(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	if (atoms[0].type == Shell::AtomInteger) {
		result.type = Shell::AtomInteger;
		result.value_int = atoms[0].value_int / atoms[1].value_int;
	} else if (atoms[0].type == Shell::AtomFloat) {
		result.type = Shell::AtomFloat;
		result.value_float = atoms[0].value_float / atoms[1].value_float;
	}

	return result;
}

Shell::Atom modulo(Shell::Atom atoms[]) {
	assert(atoms[0].type == atoms[1].type);

	Shell::Atom result;
	result.type = Shell::AtomInteger;
	result.value_int = atoms[0].value_int % atoms[1].value_int;

	return result;
}

Shell::Atom toBoolean(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomBoolean;
	result.value_bool = atoms[0].value_int > 0;

	return result;
}

Shell::Atom toInteger(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomInteger;
	result.value_int = static_cast<int>(atoms[0].value_float);

	return result;
}

Shell::Atom toFloat(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomFloat;
	result.value_float = static_cast<float>(atoms[0].value_int);

	return result;
}

Shell::Atom toString(Shell::Atom atoms[]) {
	Shell::Atom result;
	result.type = Shell::AtomString;
	result.value_string = shell.getTempString();
	switch (atoms[0].type) {
		case Shell::AtomBoolean:
			sprintf(result.value_string, "%s", atoms[0].value_bool ? "true" : "false");
			return result;
		case Shell::AtomInteger:
			sprintf(result.value_string, "%i", atoms[0].value_int);
			break;
		case Shell::AtomFloat:
			sprintf(result.value_string, "%f", atoms[0].value_float);
			break;
		default: break;
	}
	return result;
}

Shell::Atom concatenate(Shell::Atom atoms[]) {
	assert(strlen(atoms[0].value_string) + strlen(atoms[1].value_string) + 1 <= 256); // maximum tmp string size

	Shell::Atom result;
	result.type = Shell::AtomString;
	result.value_string = shell.getTempString();
	strcpy(result.value_string, atoms[0].value_string);
	strcat(result.value_string, atoms[1].value_string);
	return result;
}

Shell::Atom print(Shell::Atom atoms[]) {
	fputs(atoms[0].value_string, stdout);
	return Shell::atom_void;
}
