const GLuint BF_VA_POSITION = 0;
const GLuint BF_VA_TEXCOORD = 1;

void BitmapFont::init() {
	_texture = loadTexture2D("data/gfx/font_outline.tga", false, NULL, NULL);
	glBindTexture(GL_TEXTURE_2D, _texture);
	setFilterTexture2D(GL_NEAREST, GL_NEAREST);
	setWrapTexture2D(GL_REPEAT, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	char vert_source[] = {
		"uniform vec2 resolution;					\n"
		"attribute vec2 position;					\n"
		"attribute vec2 texcoord;					\n"
		"varying vec2 vertex_texcoord;				\n"
		"void main() {								\n"
		"	vertex_texcoord = texcoord;\n"
		"	gl_Position = vec4(2.0*position.x/resolution.x - 1.0, 1.0 - 2.0*position.y/resolution.y, 0.0, 1.0);\n"
		"}											\n"
	};

	char frag_source[] = {
		"#ifdef GL_ES								\n"
		"precision mediump float;					\n"
		"#endif										\n"
		"uniform sampler2D colormap;				\n"
		"varying vec2 vertex_texcoord;				\n"
		"void main() {								\n"
		"	gl_FragColor = texture2D(colormap, vertex_texcoord);\n"
		//"	gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
		"}											\n"
	};
	_shader.compileAndAttach(GL_VERTEX_SHADER, vert_source);
	_shader.compileAndAttach(GL_FRAGMENT_SHADER, frag_source);
	_shader.bindVertexAttrib("position", BF_VA_POSITION);
	_shader.bindVertexAttrib("texcoord", BF_VA_TEXCOORD);
	_shader.link();
	_shader.use();
	_resolution_loc = _shader.getUniformLocation("resolution");
	_texture_loc = _shader.getUniformLocation("colormap");

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, BF_MAX_VERTEX_COUNT*sizeof(BFVertex), NULL, GL_STREAM_DRAW);

	_vertex_count = 0;
}

void BitmapFont::drawText(const char *text, int x, int y) {
	int x0 = x;

	for (char *c = (char*)text; *c; c++) {
		if (*c == '\n') {
			x = x0; y += 10;
			continue;
		}

		vec2 p0 = v2(x, y);
		vec2 p1 = p0 + v2(8.0f);
		vec2 t0 = v2((float)(*c%16) / 16.0f, (float)(*c/16) / 8.0f);
		vec2 t1 = t0 + v2(1.0f / 16.0f, 1.0f / 8.0f);

		assert(_vertex_count+6 <= BF_MAX_VERTEX_COUNT);
		BFVertex *v = _vertices+_vertex_count;

		v->position = v2(p0.x, p0.y);
		v->texcoord = v2(t0.x, t0.y);
		v++;
		v->position = v2(p0.x, p1.y);
		v->texcoord = v2(t0.x, t1.y);
		v++;
		v->position = v2(p1.x, p0.y);
		v->texcoord = v2(t1.x, t0.y);
		v++;
		v->position = v2(p1.x, p0.y);
		v->texcoord = v2(t1.x, t0.y);
		v++;
		v->position = v2(p0.x, p1.y);
		v->texcoord = v2(t0.x, t1.y);
		v++;
		v->position = v2(p1.x, p1.y);
		v->texcoord = v2(t1.x, t1.y);
		v++;

		x += 6;
		_vertex_count += 6;
	}
}

int BitmapFont::getTextWidth(const char *text) {
	int width = 0;
	for (char *c = (char*)text; *c; c++) {
		width += 6;
	}
	return width;
}

void BitmapFont::render(vec2 resolution) {
	if (_vertex_count == 0) return;

	// fill vbo
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _vertex_count*sizeof(BFVertex), _vertices);

	// configure vertex array
	glEnableVertexAttribArray((GLuint)0);
	glEnableVertexAttribArray((GLuint)1);
	glVertexAttribPointer(BF_VA_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(BFVertex), (GLvoid*)0);
	glVertexAttribPointer(BF_VA_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(BFVertex), (GLvoid*)8);

	// configure shader
	_shader.use();
	glUniform2f(_resolution_loc, resolution.x, resolution.y);
	glUniform1i(_texture_loc, 0);

	glBindTexture(GL_TEXTURE_2D, _texture);

	glDrawArrays(GL_TRIANGLES, 0, _vertex_count);
	_vertex_count = 0; // flush

	glDisableVertexAttribArray(BF_VA_POSITION);
	glDisableVertexAttribArray(BF_VA_TEXCOORD);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
