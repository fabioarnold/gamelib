Shader::Shader() : _program(0), _vert_shader(0), _frag_shader(0) {
}

void Shader::free() {
	if (_program) {
		if (_vert_shader) {
			glDetachShader(_program, _vert_shader);
			glDeleteShader(_vert_shader);
			_vert_shader = 0;
		}
		if (_frag_shader) {
			glDetachShader(_program, _frag_shader);
			glDeleteShader(_frag_shader);
			_frag_shader = 0;
		}
		glDeleteProgram(_program);
		_program = 0;
	}
}

GLuint Shader::compileShader(GLuint shader_type, const char *shader_src) {
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);
	return shader;
}

bool Shader::isCompiled(GLuint shader) {
	GLint is_compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
	return is_compiled != 0;
}

void Shader::printCompileErrorLog(GLuint shader_type, GLuint shader) {
	GLint error_log_len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_log_len);
	if (error_log_len > 0) {
		char *error_log = new char[error_log_len];
		glGetShaderInfoLog(shader, error_log_len, &error_log_len, error_log);
		LOGI("Error log:\n%s", error_log);
		delete [] error_log;
	} else {
		LOGI("Empty error log.");
	}
}

void Shader::compileAndAttach(GLuint shader_type, const char *shader_src, const char *shader_filename) {
	GLuint *shader;
	switch (shader_type) {
	case GL_VERTEX_SHADER:
		shader = &_vert_shader;
		break;
	case GL_FRAGMENT_SHADER:
		shader = &_frag_shader;
		break;
	default:
		LOGE("Unsupported shader type: 0x%X.", shader_type);
		return;
	}

	if (*shader) { // shader already exists
		assert(_program != 0);
		glDetachShader(_program, *shader);
		glDeleteShader(*shader);
	}
	*shader = compileShader(shader_type, shader_src);
	if (!isCompiled(*shader)) {
		const char *shader_type_name = shader_type == GL_FRAGMENT_SHADER ? "Fragment" : "Vertex";
		LOGE("Compililation of %s shader \"%s\" failed.", shader_type_name, shader_filename);
#ifdef DEBUG
		printCompileErrorLog(shader_type, *shader);
#endif
		glDeleteShader(*shader);
		*shader = 0;
		return;
	}

	if (!_program) {
		_program = glCreateProgram();
	}
	glAttachShader(_program, *shader);
}

void Shader::readCompileAndAttach(GLuint shader_type, const char *shader_name) {
	const char *shader_src = (const char*)readStringFromFile(shader_name);
	if (!shader_src) return;

	compileAndAttach(shader_type, shader_src);

	delete [] shader_src;
}

void Shader::bindVertexAttrib(const char *name, GLuint index) {
	glBindAttribLocation(_program, index, name);
}

/*
void Shader::bindFragData(const char *name, GLuint color) {
	glBindFragDataLocation(_program, color, name);
}
*/

GLint Shader::getUniformLocation(const char *name) {
	return glGetUniformLocation(_program, name);
}

void Shader::link() {
	glLinkProgram(_program);
#ifdef DEBUG
	GLint is_linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &is_linked);
	if (!is_linked) {
		GLint link_log_len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &link_log_len);
		if (link_log_len > 0) {
			char *link_log = new char[link_log_len];
			glGetProgramInfoLog(_program, link_log_len, &link_log_len, link_log);
			LOGE("Shader program not linked. Error log:\n%s", link_log);
			delete [] link_log;
		} else {
			LOGE("Shader program not linked. Empty error log.");
		}

		free();
		return;
	}

	// validate
	glValidateProgram(_program);
	GLint is_valid;
	glGetProgramiv(_program, GL_VALIDATE_STATUS, &is_valid);
	if (!is_valid) {
		GLint validation_log_len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &validation_log_len);
		if (validation_log_len > 0) {
			char *validation_log = new char[validation_log_len];
			glGetProgramInfoLog(_program, validation_log_len, &validation_log_len, validation_log);
			LOGI("Shader program invalid. Validation log:\n%s", validation_log);
			delete [] validation_log;
		} else {
			LOGI("Shader program invalid. Empty validation log.");
		}

		free();
		return;
	}
#endif
}

void Shader::use() {
	glUseProgram(_program);
}
