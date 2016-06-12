bool shellFalse() {
	return false;
}

bool shellTrue() {
	return true;
}

bool shellNot(bool b) {
	return !b;
}

bool shellAnd(bool a, bool b) {
	return a && b;
}

bool shellOr(bool a, bool b) {
	return a || b;
}

Shell::MyFunction Shell::functions[] = {
	{"false", MAKE_FUNCTION_SIGNATURE0(TF_BOOL), (void*)&shellFalse},
	{"true", MAKE_FUNCTION_SIGNATURE0(TF_BOOL), (void*)&shellTrue},
	{"not", MAKE_FUNCTION_SIGNATURE1(TF_BOOL, TF_BOOL), (void*)&shellNot},
	{"and", MAKE_FUNCTION_SIGNATURE2(TF_BOOL, TF_BOOL, TF_BOOL), (void*)&shellAnd},
	{"or", MAKE_FUNCTION_SIGNATURE2(TF_BOOL, TF_BOOL, TF_BOOL), (void*)&shellOr},
};
