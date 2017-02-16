const u32 BF_MAX_VERTEX_COUNT = 1024;

struct BFVertex {
	vec2 position;
	vec2 texcoord;
};

struct BitmapFont {
	void init();
	void drawText(const char *text, int x, int y);
	int getTextWidth(const char *text);

	void render(vec2 resolution);

	GLuint _texture;

	BFVertex _vertices[BF_MAX_VERTEX_COUNT];
	int _vertex_count;

	Shader _shader;
	GLuint _resolution_loc;
	GLuint _texture_loc;
	GLuint _vbo;
} bitmapfont;
