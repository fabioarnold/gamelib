enum ConfigVariableType {
	Bool,
	Int,
	Float,
	String
};

struct ConfigVariable {
	ConfigVariableType type;
	const char *key;
	union {
		bool *bool_ref;
		int *int_ref;
		float *float_ref;
		struct {
			int length;
			char *value;
		} string;
	};

	void parseString(char *str) {
		switch (type) {
			case ConfigVariableType::Bool:
				*bool_ref = atoi(str) != 0;
				break;
			case ConfigVariableType::Int:
				*int_ref = atoi(str);
				break;
			case ConfigVariableType::Float:
				*float_ref = atof(str);
				break;
			case ConfigVariableType::String:
				strncpy(string.value, str, string.length);
				string.value[string.length-1] = '\0';
				break;
		}
	}
};

// modifies str while parsing
void parseConfigString(char *str, ConfigVariable *vars, int var_count) {
	char *key = str, *value = nullptr;
	for (char *c = str; *c; c++) {
		switch (*c) {
			case '\n':
				*c = '\0'; // terminate 'value'
				if (value) {
					ConfigVariable *found_var = nullptr;
					for (int i = 0; i < var_count; i++) {
						if (!strcmp(vars[i].key, key)) {
							found_var = &vars[i];
							break;
						}
					}
					if (found_var) {
						found_var->parseString(value);
					} else {
						LOGE("Could not find variable for key '%s'.", key);
					}
					value = nullptr;
				}
				key = c+1; // next key starts on next line
				break;
			case '=':
				*c = '\0'; // terminate 'key'
				value = c+1; // next value starts after '=' mark
				break;
		}
	}
}

void writeConfigFile(const char *filepath, ConfigVariable *vars, int var_count) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		LOGE("Could not write file '%s'.", filepath);
		return;
	}

	for (int i = 0; i < var_count; i++) {
		switch (vars[i].type) {
			case ConfigVariableType::Bool:
				fprintf(file, "%s=%d\n", vars[i].key, *vars[i].bool_ref);
				break;
			case ConfigVariableType::Int:
				fprintf(file, "%s=%d\n", vars[i].key, *vars[i].int_ref);
				break;
			case ConfigVariableType::Float:
				fprintf(file, "%s=%f\n", vars[i].key, *vars[i].float_ref);
				break;
			case ConfigVariableType::String:
				fprintf(file, "%s=%s\n", vars[i].key, vars[i].string.value);
				break;
		}
	}

	fclose(file);
}
