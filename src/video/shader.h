const int SHADER_TYPE_COUNT = 2; // vertex and fragment shaders

class Shader {
public:
	Shader();
	~Shader() {free();} // WARNING: makes opengl calls (needs valid opengl context)
	void destroy(); // deletes OpenGL objects
	void free();

	// will return true on success
	bool compileAndAttach(GLuint shader_type, const char *shader_src, const char *shader_filename = "");
	bool readCompileAndAttach(GLuint shader_type, const char *shader_name);
	char *getShaderCompileErrorLog(GLuint shader_type);

	void bindVertexAttrib(const char *name, GLuint index);
	// color is the index to the COLOR_ATTACHMENTi specified by glDrawBuffers
	void bindFragData(const char *name, GLuint color);

	bool link();
	char *getLinkErrorLog();

	int getUniformLocation(const char *name);

	void use();

	GLuint getProgram() {return _program;}

private:
	GLuint *getShader(GLuint shader_type); // get reference to vertex or fragment shader
	GLuint compileShader(GLuint shader_type, const char *shader_source);
	bool isCompiled(GLuint shader);
	char *getCompileErrorLog(GLuint shader_type, GLuint shader);
	bool isLinked();

	GLuint _vert_shader;
	GLuint _frag_shader;
	GLuint _program;
};
