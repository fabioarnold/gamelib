char *Shell::getTempString() {
	last_temp_string = (last_temp_string+1) % MAX_TEMP_STRING_COUNT;
	return temp_strings[last_temp_string];
}

void Shell::Atom::print() {
	switch (type) {
	case AtomVoid:
		printf("void\n");
		break;
	case AtomBoolean:
		printf(value_bool ? "true\n" : "false\n");
		break;
	case AtomInteger:
		printf("%d\n", value_int);
		break;
	case AtomFloat:
		printf("%f\n", value_float);
		break;
	case AtomString:
		printf("\"%s\"\n", value_string);
		break;
	}
}

Shell::Atom Shell::MyFunction::call(Shell::Atom args[]) {
	Shell::Atom result;
	switch (signature) {
	case MAKE_FUNCTION_SIGNATURE0(TF_BOOL):
		result.type = AtomBoolean;
		result.value_bool = (*(bool (*)(void))func_ptr)();
		break;
	case MAKE_FUNCTION_SIGNATURE1(TF_BOOL, TF_BOOL):
		result.type = AtomBoolean;
		result.value_bool = (*(bool (*)(bool))func_ptr)(args[0].value_bool);
		break;
	case MAKE_FUNCTION_SIGNATURE2(TF_BOOL, TF_BOOL, TF_BOOL):
		result.type = AtomBoolean;
		result.value_bool = (*(bool (*)(bool, bool))func_ptr)(args[0].value_bool, args[1].value_bool);
		break;
	case MAKE_FUNCTION_SIGNATURE2(TF_VOID, TF_STRING, TF_STRING):
		result.type = AtomVoid;
		(*(void (*)(const char *, const char *))func_ptr)(args[0].value_string, args[1].value_string);
		break;
	default: assert(false); // unimplemented function signature
	}

	return result;
}

#if 0
std::string unescapeString(const std::string &str) {
	std::string unescapedString;
	bool escaped = false;
	for (char chr : str) {
		if (escaped) {
			switch (chr) {
				case 'n': unescapedString.push_back('\n'); break;
				case '"': unescapedString.push_back('"'); break;
				case '\\': unescapedString.push_back('\\'); break;
				default: fprintf(stderr, "WARNING: Unknown escape: \\%c\n", chr);
			}
			escaped = false;
		} else {
			if (chr == '\\') escaped = true;
			else unescapedString.push_back(chr);
		}
	}
	return unescapedString;
}
#endif


// TODO: use a hashmap or something
int Shell::lookupSymbol(const char *symbol_name) {
	for (int i = 0; i < ARRAY_COUNT(functions); i++) {
		if (!strcmp(functions[i].name, symbol_name)) {
			return i;
		}
	}
	return -1;
}

const Shell::Atom Shell::atom_void = {AtomVoid, {0}};

enum ParserState : int {
	STATE_EXPECT_TOKEN,
	STATE_PARSE_STRING,
	STATE_PARSE_LITERAL,
	STATE_PARSE_SYMBOL,
	STATE_SKIP_LINE
};

// used by the parser
enum TokenType : int {
	TokenSymbol,	// symbol
	TokenInteger,	// 1
	TokenFloat,		// 1.0
	TokenString		// "string"
};

const int MAX_SCOPE_DEPTH = 32;
struct Compiler {
	bool compile(const char *text, ByteBuffer *bytecode);

private:
	ByteBuffer *out;

	ParserState state;

	int error_state;
	const char *error_msg;

	TokenType token_type;
	char *token_begin;

	char *line_begin;
	int line_count;

	void expectToken(char *c);

	void parseString(char *c);
	void parseLiteral(char *c);
	void parseSymbol(char *c);

	int scope_level;
	int if_locations[MAX_SCOPE_DEPTH];
	int else_locations[MAX_SCOPE_DEPTH];

	void emitIf();
	void emitElse();
	void emitEnd();

	void raiseCompileError(const char *msg);
} compiler;

void Compiler::raiseCompileError(const char *msg) {
	error_state = 1;
	error_msg = msg;
	out->clear(); // delete byte code output so it isn't used accidently
}

bool Compiler::compile(const char *text, ByteBuffer *bytecode) {
	assert(bytecode->getLength() == 0); // we need an empty output buffer
	out = bytecode;

	// for testing (result: initialization doesn't matter)
	//memset(if_locations, 0, sizeof(if_locations));
	//memset(else_locations, 0, sizeof(else_locations));

	line_begin = (char *)text;
	line_count = 0;

	error_state = 0;
	scope_level = -1;
	state = STATE_EXPECT_TOKEN;
	for (char *c = (char *)text; true; c++) {
		switch (state) {
		case STATE_EXPECT_TOKEN:
			expectToken(c);
			break;
		case STATE_PARSE_STRING:
			parseString(c);
			break;
		case STATE_PARSE_LITERAL:
			parseLiteral(c);
			break;
		case STATE_PARSE_SYMBOL:
			parseSymbol(c);
			break;
		case STATE_SKIP_LINE:
			if (*c == '\n') state = STATE_EXPECT_TOKEN;
			break;
		}

		if (error_state) {
			// output line number and underline char in line
			printf("\x1b[0;1mcompile error in line %d: %s\x1b[0m\n", line_count+1, error_msg);
			printf("\t"); for (char *d = line_begin; *d && *d != '\n'; d++) putchar(*d); putchar('\n');
			// underline
			printf("\t\x1b[31m");
			char *u = line_begin; for (; u != token_begin; u++) putchar(' ');
			/*putchar('^'); u++;*/ for (; u != c; u++) putchar('~'); printf("\x1b[0m\n");

			return false;
		}

		if (*c == '\n') {
			line_begin = c+1;
			line_count++;
		}
		// we need to check for the 0 termination here because expectToken etc
		// also need to see the 0 char to complete any open tokens
		if (!*c) break; // exit for loop
	}
	if (scope_level != -1) {
		raiseCompileError("missing 'end'");
		printf("\x1b[0;1mcompilation failed: %s\x1b[0m\n", error_msg);

		return false;
	}

	return true;
}

void Compiler::expectToken(char *c) {
	assert(state == STATE_EXPECT_TOKEN);
	switch (*c) {
	case ' ': case '\t': case '\r': case '\n': case '\0': // whitespace
		return;
	case '#':
		state = STATE_SKIP_LINE;
		break;
	case '"': // string
		token_begin = c;
		token_type = TokenString;
		state = STATE_PARSE_STRING;
		break;
	case '-':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9': // integer':
		token_begin = c;
		token_type = TokenInteger;
		state = STATE_PARSE_LITERAL;
		break;
	default: // symbol
		token_begin = c;
		token_type = TokenSymbol;
		state = STATE_PARSE_SYMBOL;
	}
}

void Compiler::parseString(char *c) {
	assert(state == STATE_PARSE_STRING);
	switch (*c) {
	case '\n': case '\0':
		raiseCompileError("unexpected end of string");
		return;
	case '\\':
		// TODO: handle escaping
		break;
	case '"':
		int len = (int)(c-(token_begin+1));
		if (len < 0 && len > 255) {
			raiseCompileError("string too long");
			return;
		}
		out->appendUInt8(ByteCodeString);
		out->appendUInt8(len);
		out->appendData((uint8_t*)(token_begin+1), len);

		state = STATE_EXPECT_TOKEN;
		break;
	}
}

void Compiler::parseLiteral(char *c) {
	assert(state == STATE_PARSE_LITERAL);
	switch (*c) {
	case ' ': case '\t': case '\r': case '\n': case '\0': { // whitespace
		int len = (int)(c-token_begin);
		if (len > 255) {
			raiseCompileError("literal longer than 255 characters");
			return;
		}
		char *literal_str = shell.getTempString();
		strncpy(literal_str, token_begin, len);
		literal_str[len] = '\0';

		if (token_type == TokenInteger) {
			out->appendUInt8(ByteCodeInteger);
			out->appendSInt32LE(atoi(literal_str));
		} else if (token_type == TokenFloat) {
			union {
				int value_int;
				float value_float;
			};
			value_float = atof(literal_str);
			out->appendUInt8(ByteCodeFloat);
			out->appendSInt32LE(value_int);
		}

		state = STATE_EXPECT_TOKEN;
		break;
	}
	case '.':
		token_type = TokenFloat;
		break;
	}
}

void Compiler::parseSymbol(char *c) {
	switch (*c) {
	case ' ': case '\t': case '\r': case '\n': case '\0': // whitespace
		int len = (int)(c-token_begin);
		if (len > 255) {
			raiseCompileError("symbol name longer than 255 characters");
			return;
		}
		char *symbol_name = shell.getTempString();
		strncpy(symbol_name, token_begin, len);
		symbol_name[len] = '\0';

		if (!strcmp(symbol_name, "if")) {
			emitIf();
		} else if (!strcmp(symbol_name, "else")) {
			emitElse();
		} else if (!strcmp(symbol_name, "end")) {
			emitEnd();
		} else {
			int symbol_index = shell.lookupSymbol(symbol_name);
			if (symbol_index == -1) {
				raiseCompileError("undefined symbol");
				return;
			}
			out->appendUInt8(ByteCodeSymbol);
			out->appendUInt8(symbol_index);
		}

		state = STATE_EXPECT_TOKEN;
		break;
	}
}

void Compiler::emitIf() {
	if (scope_level+1 >= MAX_SCOPE_DEPTH) {
		raiseCompileError("exceded maximum scope level (too many ifs)");
		return;
	}
	out->appendUInt8(ByteCodeCondJump); // insert conditional jump
	if_locations[++scope_level] = out->getLength();
	out->appendSInt32LE(0); // reserve location for else or end statement
}

void Compiler::emitElse() {
	if (scope_level < 0) {
		raiseCompileError("'else' without matching 'if'");
		return;
	}
	// insert unconditional jump
	out->appendUInt8(ByteCodeJump);
	else_locations[scope_level] = out->getLength();
	// reserve location for end statement
	out->appendSInt32LE(0);
	int jump_pos = out->getLength();
	int instr_loc = if_locations[scope_level];
	if (out->getSInt32LE(instr_loc)) { // jump pos already set
		raiseCompileError("'else' without matching 'if'");
		return;
	}
	out->setSInt32LE(instr_loc, jump_pos);
	// indicate that the if part is complete
	if_locations[scope_level] = -1;
}

void Compiler::emitEnd() {
	if (scope_level < 0) {
		raiseCompileError("'end' without matching 'if'");
		return;
	}
	int jump_pos = out->getLength();
	int instr_loc = 0;
	if (if_locations[scope_level] == -1) { // there was an else part
		instr_loc = else_locations[scope_level];
	} else { // no else part
		instr_loc = if_locations[scope_level];
	}
	assert(instr_loc >= 0 && instr_loc < out->getLength());
	if (out->getSInt32LE(instr_loc)) { // jump pos already set
		raiseCompileError("'end' without matching 'if'");
		return;
	}
	// complete the jump instr
	out->setSInt32LE(instr_loc, jump_pos);
	scope_level--;
}

// returns true on success
bool Shell::compile(const char *text, ByteBuffer *bytecode) {
	return compiler.compile(text, bytecode);
}

void testCompiler() {
	const char *error_programs[] = {
		"if", // if without end
		"end", // end without if
		"if else", // else without end
		"else end", // else without if
		"else", // else without if and end
		"if if end", // if without end
		"if else else end", // else without if
		"if end end", // end without if

		"\"hello", // unexpected end of string
	};
	ByteBuffer bytecode;
	for (int i = 0; i < ARRAY_COUNT(error_programs); i++) {
		bytecode.clear();
		if (compiler.compile(error_programs[i], &bytecode)) { // compile was successful
			LOGW("expected program #%d to not compile:\n%s", i, error_programs[i]);
		}
	}
}

// recursive
Shell::Atom Shell::evaluate(ByteBuffer *bytecode, int *ip) {
	Shell::Atom result;
	Shell::Atom arguments[4];

	assert(*ip < bytecode->getLength());

	switch (bytecode->getUInt8((*ip)++)) {
	case ByteCodeSymbol: { // (function)
		int opCode = bytecode->getUInt8(*ip); *ip += 1;
		int numArguments = (functions[opCode].signature>>4)&3;
		//int numArguments = _builtinFunctions[opCode].function.numArguments;
		for (int i = 0; i < numArguments; i++) {
			arguments[i] = evaluate(bytecode, ip);
		}
		return functions[opCode].call(arguments);
		//return _builtinFunctions[opCode].function.function(arguments);
	}
	case ByteCodeInteger:
		result.type = AtomInteger;
		result.value_int = bytecode->getSInt32LE(*ip); *ip += 4;
		break;
	case ByteCodeFloat:
		result.type = AtomFloat;
		result.value_int = bytecode->getSInt32LE(*ip); *ip += 4;
		break;
	case ByteCodeString: {
		result.type = AtomString;
		result.value_string = getTempString();
		int stringLength = bytecode->getUInt8(*ip); *ip += 1;
		memcpy(result.value_string, bytecode->getData(*ip), stringLength); *ip += stringLength;
		result.value_string[stringLength] = '\0'; // zero termination
	}	break;
	case ByteCodeCondJump: {
		int jumpIP = bytecode->getSInt32LE(*ip); *ip += 4;
		Atom expression = evaluate(bytecode, ip);
		if (!expression.value_bool) {
			*ip = jumpIP;
		}
	}	break;
	case ByteCodeJump: {
		int jumpIP = bytecode->getSInt32LE(*ip); *ip += 4;
		*ip = jumpIP;
	}	break;
	}

	return result;
}

void Shell::executeFile(const char *filepath) {
	if (char *text = readStringFromFile(filepath)) {
		execute((const char*)text);
		delete [] text;
	}
}

void Shell::execute(const char *text) {
	ByteBuffer bytecode;
	if (compile(text, &bytecode)) {
#if 0
		// output byte code
		for (int i = 0; i < bytecode.getLength(); i++) {
			printf("%02X ", bytecode._data[i]);
		}
		printf("\n");
#endif
		int instruction_pointer = 0;
		execute(&bytecode, &instruction_pointer);
	}
}

void Shell::execute(ByteBuffer *bytecode, int *ip) {
	interrupted = false;
	while (!interrupted && *ip < bytecode->getLength()) {
		evaluate(bytecode, ip);
	}
}
