const int DR_MAX_VERTEX_COUNT = 65536;
const int DR_VERTEX_SIZE = 24;
const int DR_MAX_VERTEX_BUFFER_SIZE = DR_MAX_VERTEX_COUNT*DR_VERTEX_SIZE;
const int DR_MAX_BATCH_COUNT = 256;

struct DebugRenderBatch {
	GLenum primitive; // GL_LINES or GL_TRIANGLES
	vec4 color;
	GLuint texture;
	int offset;
	int count;
};

struct DebugRenderer {
	void setColor(float r, float g, float b, float a);
	void drawLine(vec2 v0, vec2 v1);
	void drawLine(vec3 v0, vec3 v1);
	void drawLine(vec3 *v);
	void drawTriangle(vec3 v0, vec3 v1, vec3 v2);
	void drawTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 n);
	void drawTriangle(vec3 *v);
	void drawImage(GLuint texture, vec2 r0, vec2 r1);

	void init();
	void render(mat4 view_proj_mat); // renders all the draw calls
	void destroy();

	vec4 _color;

	DebugRenderBatch _batches[DR_MAX_BATCH_COUNT];
	int _batch_count;

	vec3 _vertex_positions[DR_MAX_VERTEX_COUNT];
	vec3 _vertex_normals[DR_MAX_VERTEX_COUNT];
	int _vertex_count;

	Shader _shader;
	GLint _mvp_loc;
	GLint _color_loc;
	GLuint _vbo;
	Shader tex_shader;
} debug_renderer;

void debugDrawMatrix(mat4 m, float scale);
void debugDrawAxis(vec3 p);

// debug draw stuff
void drawLineStrip(vec3 *points, int num_points);
void drawFan(vec3 *points, int num_points);
void drawQuad(vec3 p0, vec3 p1, vec3 p2, vec3 p3);
void drawCircle(vec2 center, float radius); // filled
