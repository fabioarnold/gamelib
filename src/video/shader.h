class Shader {
public:
	Shader();
	~Shader() {free();} // WARNING: makes opengl calls

	void free();

	void compileAndAttach(GLuint shader_type, const char *shader_src, const char *shader_filename = "");
	void readCompileAndAttach(GLuint shader_type, const char *shader_name);

	void bindVertexAttrib(const char *name, GLuint index);
	// color is the index to the COLOR_ATTACHMENTi specified by glDrawBuffers
	void bindFragData(const char *name, GLuint color);

	void link();

	int getUniformLocation(const char *name);

	void use();

	GLuint getProgram() {return _program;}

private:
	GLuint compileShader(GLuint shader_type, const char *shader_source);
	bool isCompiled(GLuint shader);
	void printCompileErrorLog(GLuint shader_type, GLuint shader);

	GLuint _vert_shader;
	GLuint _frag_shader;
	GLuint _program;
};
