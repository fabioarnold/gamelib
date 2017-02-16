const int MAX_TEMP_STRING_COUNT = 32;

// byte code stuff
enum ByteCodeOp : u8 {
	ByteCodeSymbol,
	ByteCodeInteger,
	ByteCodeFloat,
	ByteCodeString,
	ByteCodeCondJump,
	ByteCodeJump // unconditionally
};

enum DebugTypeFlags : u64 {
	FlagVoid   = 0x0,
	FlagBool   = 0x1,
	FlagInt    = 0x2,
	FlagFloat  = 0x4,
	FlagString = 0x8
};

struct Shell {
	enum AtomType : u8 {
		AtomVoid,
		AtomBoolean,
		AtomInteger,
		AtomFloat,
		AtomString
	};

	struct Atom {
		AtomType type;
		union {
			bool value_bool;
			int value_int;
			float value_float;
			char *value_string; // temporary static string. DON'T delete!
			//Atom (*functionPointer)(Atom *args[]);
		};

		void print(); // debug
	};
	static const Atom atom_void;

	struct Function {
		// const char *name
		unsigned numArguments;
		Atom (*function)(Atom args[]);
#ifdef DEBUG
		int typeDebugFlags;
#endif
	};

	struct BuiltinFunction {
		const char *name;
		Function function;
		/*
		unsigned numArguments;
		Atom (*function)(Atom args[]);
		*/
	};


	bool compile(const char *text, ByteBuffer *bytecode);

	void interrupt() {interrupted = true;}
	void execute(ByteBuffer *bytecode, int *instruction_pointer); // instruction pointer will be updated
	void execute(const char *text);
	void executeFile(const char *filepath);

	// used by compiler
	int lookupSymbol(const char *symbol_name); // returns -1 if symbol couldn't be found

	char *getTempString();

private:
	enum TypeFlags : u64 {
		TF_VOID   = 0x0,
		TF_BOOL   = 0x1,
		TF_INT    = 0x2,
		TF_FLOAT  = 0x4,
		TF_STRING = 0x8
	};

	struct MyFunction {
		const char *name;
		u64 signature; // 0-3 return type
		void *func_ptr;

		Shell::Atom call(Shell::Atom args[]);
	};

	#define MAKE_FUNCTION_SIGNATURE0(ret_type) \
		((ret_type) | (0)<<4)
	#define MAKE_FUNCTION_SIGNATURE1(ret_type, arg0) \
		((ret_type) | (1)<<4 | (arg0)<<8)
	#define MAKE_FUNCTION_SIGNATURE2(ret_type, arg0, arg1) \
		((ret_type) | (2)<<4 | (arg0)<<8 | (arg1)<<12)
	#define MAKE_FUNCTION_SIGNATURE3(ret_type, arg0, arg1, arg2) \
		((ret_type) | (3)<<4 | (arg0)<<8 | (arg1)<<12 | (arg2)<<16)
	#define MAKE_FUNCTION_SIGNATURE4(ret_type, arg0, arg1, arg2, arg3) \
		((ret_type) | (4)<<4 | (arg0)<<8 | (arg1)<<12 | (arg2)<<16 | (arg3)<<20)

	static MyFunction functions[];

	//static BuiltinFunction _builtinFunctions[];


	bool interrupted; // if this is true current execution will be interrupted

	char temp_strings[256][MAX_TEMP_STRING_COUNT];
	int last_temp_string;

	Atom evaluate(ByteBuffer *bytecode, int *instruction_pointer);
} shell;
