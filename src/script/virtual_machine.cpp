VirtualMachine::VirtualMachine() :
	current_program(nullptr),
	paused(false),
	instruction_pointer(0),
	sleep_time(0.0f)
/*
	_instructionCounter(0),
	_skipFlags(0x0),
	_nestingLevel(-1)
 */
{
}

void VirtualMachine::reset() {
	current_program = nullptr;
	paused = false;
	instruction_pointer = 0;
	sleep_time = 0.0f;
/*
	_instructionCounter = 0;
	_skipFlags = 0x0;
	_nestingLevel = -1;
 */
}

void VirtualMachine::setProgram(ByteBuffer *program) {
	reset();
	current_program = program;
}

void VirtualMachine::pause() {
	paused = true;
	shell.interrupt(); // interrupt execution
}

void VirtualMachine::resume() {
	paused = false;
}

bool VirtualMachine::executionStopped() {
	return current_program == nullptr || !(instruction_pointer < current_program->getLength());
}

void VirtualMachine::execute() {
	if (executionStopped() || paused) return;

	shell.execute(current_program, &instruction_pointer);
}

void VirtualMachine::tick(float delta_time) {
	if (sleep_time > 0.0f) {
		sleep_time -= delta_time;
		if (sleep_time <= 0.0f) {
			sleep_time = 0.0f;
			resume();
		}
	}

	execute();
}

/* execute text based script

	int _instructionCounter; // basically the line number

	int _skipFlags; // updated when if conditions get evaluated
	int _nestingLevel; // has to be < 32

void VirtualMachine::execute() {
	if (executionStopped()) return;

	do {
		std::list<Shell::Token> tokenList = Shell::tokenize(current_program->text[_instructionCounter++]);
		if (tokenList.empty()) continue;

		Shell::Token token = tokenList.front();
		if (token.value == "end") {
			assert(_nestingLevel >= 0); // no matching if
			_nestingLevel--;
			continue; // next line
		} else if (token.value == "else") {
			assert(_nestingLevel >= 0); // no matching if
			_skipFlags ^= 1 << _nestingLevel; // toggle skip flag
			continue; // next line
		}
		if (_nestingLevel >= 0 && _skipFlags & (1 << _nestingLevel)) // skip instruction
			continue;

		if (token.value == "if") {
			_nestingLevel++;
			tokenList.pop_front();
			Shell::Atom result = Shell::evaluate(tokenList);
			assert(result.type == Shell::AtomBoolean);
			if (result.booleanValue) _skipFlags &= ~(1 << _nestingLevel);
			else _skipFlags |= 1 << _nestingLevel; // set skip flag
		} else {
			Shell::evaluate(tokenList);
		}
	} while (!_paused && !executionStopped());
}
*/
