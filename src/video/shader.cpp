Shader::Shader() : _program(0), _vert_shader(0), _frag_shader(0) {
}

void Shader::free() {
	// according to spec opengl automatically detaches shaders when deleting programs. i'm just being explicit
	if (_program) {
		GLuint attached_shaders[SHADER_TYPE_COUNT];
		GLsizei attached_shaders_count;
		glGetAttachedShaders(_program, SHADER_TYPE_COUNT, &attached_shaders_count, attached_shaders);
		for (int i = 0; i < attached_shaders_count; i++) {
			glDetachShader(_program, attached_shaders[i]);
		}
		glDeleteProgram(_program);
		_program = 0;
	}
	if (_vert_shader) {
		glDeleteShader(_vert_shader);
		_vert_shader = 0;
	}
	if (_frag_shader) {
		glDeleteShader(_frag_shader);
		_frag_shader = 0;
	}
}

GLuint *Shader::getShader(GLuint shader_type) {
	switch (shader_type) {
	case GL_VERTEX_SHADER:   return &_vert_shader;
	case GL_FRAGMENT_SHADER: return &_frag_shader;
	default:
		LOGE("Unsupported shader type: 0x%04X.", shader_type);
		return nullptr;
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

char *Shader::getCompileErrorLog(GLuint shader_type, GLuint shader) {
	GLint error_log_len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_log_len);

	char *error_log = nullptr;
	if (error_log_len > 0) {
		error_log = new char[error_log_len];
		glGetShaderInfoLog(shader, error_log_len, &error_log_len, error_log);
	}
	return error_log;
}

char *Shader::getShaderCompileErrorLog(GLuint shader_type) {
	GLuint *shader = getShader(shader_type);
	if (!shader || !*shader) return nullptr;
	if (isCompiled(*shader)) return nullptr;
	return getCompileErrorLog(shader_type, *shader);
}

bool Shader::compileAndAttach(GLuint shader_type, const char *shader_src, const char *shader_filename) {
	GLuint *shader = getShader(shader_type);
	if (!shader) return false;

	if (*shader) { // shader already exists
		assert(_program != 0);
		// if the shader was attached to the program detach it
		// detaching an unattached shader will raise GL_INVALID_OPERATION
		GLuint attached_shaders[SHADER_TYPE_COUNT];
		GLsizei attached_shaders_count;
		glGetAttachedShaders(_program, SHADER_TYPE_COUNT, &attached_shaders_count, attached_shaders);
		for (int i = 0; i < attached_shaders_count; i++) {
			if (*shader == attached_shaders[i]) {
				glDetachShader(_program, *shader);
				break;
			}
		}
		glDeleteShader(*shader);
	}
	*shader = compileShader(shader_type, shader_src);
	if (!isCompiled(*shader)) {
		const char *shader_type_name = shader_type == GL_FRAGMENT_SHADER ? "Fragment" : "Vertex";
		LOGE("Compililation of %s shader \"%s\" failed.", shader_type_name, shader_filename);
#ifdef DEBUG
		char *error_log = getCompileErrorLog(shader_type, *shader);
		if (error_log) {
			LOGI("Error log:\n%s", error_log);
			delete [] error_log;
		} else {
			LOGI("Empty error log.");
		}
#endif
		return false;
	}

	if (!_program) {
		_program = glCreateProgram();
	}
	glAttachShader(_program, *shader);

	return true; // success
}

bool Shader::readCompileAndAttach(GLuint shader_type, const char *shader_name) {
	const char *shader_src = (const char*)readStringFromFile(shader_name);
	if (!shader_src) return false;

	bool result = compileAndAttach(shader_type, shader_src, shader_name);

	delete [] shader_src;

	return result;
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

bool Shader::link() {
	glLinkProgram(_program);

	GLint is_linked;
	glGetProgramiv(_program, GL_LINK_STATUS, &is_linked);
	if (!is_linked) {
		LOGE("Linking of shader program failed.");
#ifdef DEBUG
		GLint link_log_len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &link_log_len);
		if (link_log_len > 0) {
			char *link_log = new char[link_log_len];
			glGetProgramInfoLog(_program, link_log_len, &link_log_len, link_log);
			LOGE("Error log:\n%s", link_log);
			delete [] link_log;
		} else {
			LOGE("Empty error log.");
		}
#endif

		free();
		return false;
	}

	// validation
	glValidateProgram(_program);

	GLint is_valid;
	glGetProgramiv(_program, GL_VALIDATE_STATUS, &is_valid);
	if (!is_valid) {
		LOGE("Validation of shader program failed.");
#ifdef DEBUG
		GLint validation_log_len;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &validation_log_len);
		if (validation_log_len > 0) {
			char *validation_log = new char[validation_log_len];
			glGetProgramInfoLog(_program, validation_log_len, &validation_log_len, validation_log);
			LOGI("Validation log:\n%s", validation_log);
			delete [] validation_log;
		} else {
			LOGI("Empty validation log.");
		}
#endif

		free();
		return false;
	}

	return true; // success
}

void Shader::use() {
	glUseProgram(_program);
}
