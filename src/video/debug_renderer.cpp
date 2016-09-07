const GLuint DR_VA_POSITION = 0;
const GLuint DR_VA_NORMAL = 1;

void DebugRenderer::init() {
	char vert_source[] = {
		"uniform mat4 mvp;							\n"
		"attribute vec3 position;					\n"
		"attribute vec3 normal;						\n"
		"varying vec3 v_normal;						\n"
		"void main() {								\n"
		"	v_normal = normal;						\n"
		"	gl_Position = mvp * vec4(position, 1.0);\n"
		"}											\n"
	};

	char frag_source[] = {
		"#ifdef GL_ES								\n"
		"precision mediump float;					\n"
		"#endif										\n"
		"uniform vec4 color;						\n"
		"varying vec3 v_normal;						\n"
		"void main() {								\n"
		"	vec3 light = vec3(0.0, 0.0, -1.0);		\n"
		"	vec3 normal = normalize(v_normal);		\n"
		"	float shade = 0.75 + 0.25*dot(normal, -light);\n"
		"	gl_FragColor = vec4(shade*color.rgb, color.a);\n"
		"}											\n"
	};
	_shader.compileAndAttach(GL_VERTEX_SHADER, vert_source);
	_shader.compileAndAttach(GL_FRAGMENT_SHADER, frag_source);
	_shader.bindVertexAttrib("position", DR_VA_POSITION);
	_shader.bindVertexAttrib("normal", DR_VA_NORMAL);
	_shader.link();
	_shader.use();
	_mvp_loc = _shader.getUniformLocation("mvp");
	_color_loc = _shader.getUniformLocation("color");

	char tex_vert_src[] =
		"attribute vec3 position;					\n"
		"attribute vec3 normal;						\n"
		"varying vec2 uv;							\n"
		"void main() {								\n"
		"	uv = normal.xy;							\n"
		"	gl_Position = vec4(position, 1.0);		\n"
		"}											\n"
	;

	char tex_frag_src[] =
		"#ifdef GL_ES								\n"
		"precision mediump float;					\n"
		"#endif										\n"
		"uniform sampler2D colormap;				\n"
		"varying vec2 uv;							\n"
		"void main() {								\n"
		"	gl_FragColor = vec4(texture2D(colormap, uv).rgb, 0.5);	\n"
		"}											\n"
	;
	tex_shader.compileAndAttach(GL_VERTEX_SHADER, tex_vert_src);
	tex_shader.compileAndAttach(GL_FRAGMENT_SHADER, tex_frag_src);
	tex_shader.bindVertexAttrib("position", DR_VA_POSITION);
	tex_shader.link();
	tex_shader.use();
	glUniform1i(tex_shader.getUniformLocation("colormap"), 0); // active tex 0


	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, DR_MAX_VERTEX_BUFFER_SIZE, NULL, GL_STREAM_DRAW);

	_vertex_count = 0;
	_batch_count = 0;
}

void DebugRenderer::destroy() {
	_shader.free();
	tex_shader.free();
}

void DebugRenderer::setColor(float r, float g, float b, float a) {
	_color.r = r;
	_color.g = g;
	_color.b = b;
	_color.a = a;
}

bool operator!=(vec4 v0, vec4 v1) {
	return v0.x != v1.x || v0.y != v1.y || v0.z != v1.z || v0.w != v1.w;
}

void DebugRenderer::drawTriangle(vec3 v0, vec3 v1, vec3 v2) {
	vec3 n = normalize(cross(v1-v0, v2-v0));
	drawTriangle(v0, v1, v2, n);
}

void DebugRenderer::drawTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 n) {
	assert(_vertex_count+3 < DR_MAX_VERTEX_COUNT);
	DebugRenderBatch *b;
	if (_batch_count == 0) {
		b = _batches;
		b->primitive = GL_TRIANGLES;
		b->color = _color;
		b->texture = 0;
		b->offset = _vertex_count;
		b->count = 3;
		_batch_count = 1;
	} else {
		b = _batches+(_batch_count-1);
		if (b->primitive != GL_TRIANGLES || b->color != _color || b->texture != 0) {
			assert(_batch_count+1 < DR_MAX_BATCH_COUNT);
			b++;
			b->primitive = GL_TRIANGLES;
			b->color = _color;
			b->texture = 0;
			b->offset = _vertex_count;
			b->count = 3;
			_batch_count++;
		} else {
			b->count += 3;
		}
	}
	_vertex_positions[_vertex_count+0] = v0;
	_vertex_positions[_vertex_count+1] = v1;
	_vertex_positions[_vertex_count+2] = v2;
	_vertex_normals[_vertex_count+0] = n;
	_vertex_normals[_vertex_count+1] = n;
	_vertex_normals[_vertex_count+2] = n;
	_vertex_count += 3;
}

void DebugRenderer::drawLine(vec2 v0, vec2 v1) {
	drawLine(v3(v0), v3(v1));
}

void DebugRenderer::drawLine(vec3 v0, vec3 v1) {
	assert(_vertex_count+2 < DR_MAX_VERTEX_COUNT);
	DebugRenderBatch *b;
	if (_batch_count == 0) {
		b = _batches;
		b->primitive = GL_LINES;
		b->color = _color;
		b->texture = 0;
		b->offset = _vertex_count;
		b->count = 2;
		_batch_count = 1;
	} else {
		b = _batches+(_batch_count-1);
		if (b->primitive != GL_LINES || b->color != _color || b->texture != 0) {
			assert(_batch_count+1 < DR_MAX_BATCH_COUNT);
			b++;
			b->primitive = GL_LINES;
			b->color = _color;
			b->texture = 0;
			b->offset = _vertex_count;
			b->count = 2;
			_batch_count++;
		} else {
			b->count += 2;
		}
	}
	_vertex_positions[_vertex_count+0] = v0;
	_vertex_positions[_vertex_count+1] = v1;
	_vertex_normals[_vertex_count+0] = v3(0.0f);
	_vertex_normals[_vertex_count+1] = v3(0.0f);
	_vertex_count += 2;
}

void DebugRenderer::drawImage(GLuint texture, vec2 r0, vec2 r1) {
	assert(_vertex_count+6 < DR_MAX_VERTEX_COUNT);
	assert(_batch_count+1 < DR_MAX_BATCH_COUNT);

	DebugRenderBatch *b = _batches+_batch_count;
	b->primitive = GL_TRIANGLES;
	b->color = v4(1.0f);
	b->texture = texture;
	b->offset = _vertex_count;
	b->count = 6;
	_batch_count++;

	_vertex_positions[_vertex_count+0] = v3(r0.x, r0.y, 0.0f);
	_vertex_positions[_vertex_count+1] = v3(r1.x, r0.y, 0.0f);
	_vertex_positions[_vertex_count+2] = v3(r0.x, r1.y, 0.0f);
	_vertex_positions[_vertex_count+3] = v3(r1.x, r0.y, 0.0f);
	_vertex_positions[_vertex_count+4] = v3(r1.x, r1.y, 0.0f);
	_vertex_positions[_vertex_count+5] = v3(r0.x, r1.y, 0.0f);
	_vertex_normals[_vertex_count+0] = v3(0.0f, 0.0f, 0.0f);
	_vertex_normals[_vertex_count+1] = v3(1.0f, 0.0f, 0.0f);
	_vertex_normals[_vertex_count+2] = v3(0.0f, 1.0f, 0.0f);
	_vertex_normals[_vertex_count+3] = v3(1.0f, 0.0f, 0.0f);
	_vertex_normals[_vertex_count+4] = v3(1.0f, 1.0f, 0.0f);
	_vertex_normals[_vertex_count+5] = v3(0.0f, 1.0f, 0.0f);
	_vertex_count += 6;
}

void DebugRenderer::render(mat4 view_proj_mat) {
	if (_vertex_count == 0) return;

	//glDisable(GL_DEPTH_TEST);

	// fill vbo
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _vertex_count*DR_VERTEX_SIZE, _vertex_positions);
	size_t normals_offset = _vertex_count*DR_VERTEX_SIZE;
	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, _vertex_count*DR_VERTEX_SIZE, _vertex_normals);
	_vertex_count = 0;

	// configure vertex array
	glEnableVertexAttribArray((GLuint)DR_VA_POSITION);
	glVertexAttribPointer((GLuint)DR_VA_POSITION, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray((GLuint)DR_VA_NORMAL);
	glVertexAttribPointer((GLuint)DR_VA_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)normals_offset);

	// configure shader
	_shader.use();
	glUniformMatrix4fv(_mvp_loc, 1, GL_FALSE, view_proj_mat.e);

	// draw all batches
	bool using_texture = _batch_count > 0 && _batches[0].texture != 0;
	for (int ib = 0; ib < _batch_count; ib++) {
		DebugRenderBatch *b = _batches+ib;
		if (using_texture != b->texture != 0) {
			using_texture = b->texture != 0;
			if (using_texture) {
				tex_shader.use();
				glBindTexture(GL_TEXTURE_2D, b->texture);
			} else _shader.use();
		}
		glUniform4f(_color_loc, b->color.r, b->color.g, b->color.b, b->color.a);
		glDrawArrays(b->primitive, b->offset, b->count);
	}
	_batch_count = 0; // flush

	glDisableVertexAttribArray((GLuint)DR_VA_POSITION);
	glDisableVertexAttribArray((GLuint)DR_VA_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glEnable(GL_DEPTH_TEST);
}

void debugDrawAxis(vec3 p) {
	debug_renderer.setColor(1.0f, 0.0f, 0.0f, 1.0f);
	debug_renderer.drawLine(p - v3(1.0f, 0.0f, 0.0f), p + v3(1.0f, 0.0f, 0.0f));
	debug_renderer.setColor(0.0f, 1.0f, 0.0f, 1.0f);
	debug_renderer.drawLine(p - v3(0.0f, 1.0f, 0.0f), p + v3(0.0f, 1.0f, 0.0f));
	debug_renderer.setColor(0.0f, 0.0f, 1.0f, 1.0f);
	debug_renderer.drawLine(p - v3(0.0f, 0.0f, 1.0f), p + v3(0.0f, 0.0f, 1.0f));
}

void drawLineStrip(vec3 *points, int num_points) {
	for (int i = 1; i < num_points; i++) {
		debug_renderer.drawLine(points[i-1], points[i]);
	}
}

void drawFan(vec3 *points, int num_points) {
	for (int i = 2; i < num_points; i++) {
		debug_renderer.drawTriangle(points[0], points[i-1], points[i]);
	}
}

void drawQuad(vec3 p0, vec3 p1, vec3 p2, vec3 p3) {
	vec3 point_list[4] = {p0, p1, p2, p3};
	drawFan(point_list, 4);
}

void drawCircle(vec2 center, float radius) { // filled
	const int detail = 32;
	const float angle_step = 2.0f * (float)M_PI / (float)detail;

	vec3 points[detail];
	for (int i = 0; i < detail; i++) {
		points[i].x = center.x + radius*cosf((float)i * angle_step);
		points[i].y = center.y + radius*sinf((float)i * angle_step);
		points[i].z = 0.0f;
	}
	drawFan(points, detail);
}
