#if 0
constexpr int magic_to_int(const char *magic) {
	return magic[0] | magic[1]<<8 | magic[2]<<16 | magic[3]<<24;
}
#endif

#if 0
MDLVertexFormat::MDLVertexFormat() {
	memset(attribs, 0, sizeof(attribs));
}
#endif

mat4 MDLBoneTransform::getMatrix() {
	return m4(translation, rotation, v3(scale));
}

size_t MDLVertexFormat::getSize() {
	size_t size = 0;
	for (GLuint attrib = 0; attrib < VAT_COUNT; attrib++) {
		size += dtSizes[attribs[attrib].data_type]
			* attribs[attrib].components_count;
	}
	return size;
}

size_t MDLVertexFormat::getAttribOffset(GLuint attribType) {
	size_t offset = 0;
	for (GLuint attrib = 0; attrib < attribType; attrib++) {
		offset += dtSizes[attribs[attrib].data_type]
			* attribs[attrib].components_count;
	}
	return offset;
}

void MDLVertexFormat::enable(void *base_pointer) {
	size_t vertexSize = getSize();
	for (GLuint attrib = 0; attrib < VAT_COUNT; attrib++) {
		if (attribs[attrib].components_count == 0) continue;
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib,
			attribs[attrib].components_count,
			dtGLTypes[attribs[attrib].data_type], GL_FALSE,
			(GLsizei)vertexSize, (GLvoid*)((size_t)base_pointer+getAttribOffset(attrib)));
	}
}

void MDLVertexFormat::disable() {
	for (GLuint attrib = 0; attrib < VAT_COUNT; attrib++) {
		if (attribs[attrib].components_count == 0) continue;
		glDisableVertexAttribArray(attrib);
	}
}

const char *mdlVertexAttribNames[] = {
	"va_position", // VAT_POSITION
	"va_normal", // VAT_NORMAL
	"va_tangent", // VAT_TANGENT
	"va_texcoord0", // VAT_TEXCOORD0
	"va_texcoord1", // VAT_TEXCOORD1
	"va_color", // VAT_COLOR
	"va_bone_indices", // VAT_BONE_INDEX
	"va_bone_weights", // VAT_BONE_WEIGHT
};
void MDLVertexFormat::bindShaderAttribs(Shader *shader) {
	for (GLuint attrib = 0; attrib < VAT_COUNT; attrib++) {
		if (attribs[attrib].components_count > 0) {
			shader->bindVertexAttrib(mdlVertexAttribNames[attrib], attrib);
		}
	}
}



// shader code generators

#include <sstream> // TODO: write our own mem efficient string builder class

char *generateVertexShaderSource(MDLVertexFormat *vertex_format) {
	std::stringstream buffer;
	buffer << "uniform mat4 mvp;" << std::endl;
	buffer << "attribute vec3 va_position;" << std::endl;

	if (vertex_format->attribs[VAT_NORMAL].components_count) {
		buffer << "attribute vec3 va_normal;" << std::endl;
		buffer << "varying vec3 v_normal;" << std::endl;
	}
	if (vertex_format->attribs[VAT_TEXCOORD0].components_count) {
		buffer << "attribute vec2 va_texcoord0;" << std::endl;
		buffer << "varying vec2 v_texcoord0;" << std::endl;
	}
	if (vertex_format->attribs[VAT_BONE_INDEX].components_count == 4) {
		buffer << "uniform mat4 bone_mats[16];" << std::endl;
		buffer << "attribute vec4 va_bone_indices;" << std::endl;
	}
	if (vertex_format->attribs[VAT_BONE_WEIGHT].components_count == 3) {
		buffer << "attribute vec3 va_bone_weights;" << std::endl;
	}

	buffer << "void main() {" << std::endl;

	if (vertex_format->attribs[VAT_NORMAL].components_count) {
		buffer << "\tv_normal = va_normal;" << std::endl;
	}
	if (vertex_format->attribs[VAT_TEXCOORD0].components_count) {
		buffer << "\tv_texcoord0 = va_texcoord0;" << std::endl;
	}
	if (vertex_format->attribs[VAT_BONE_INDEX ].components_count == 4
	 && vertex_format->attribs[VAT_BONE_WEIGHT].components_count == 3) {
		buffer << "\tivec4 bone_indices = ivec4(va_bone_indices);" << std::endl;
		buffer << "\tfloat bone_weight3 = 1.0 - va_bone_weights[0] - va_bone_weights[1] - va_bone_weights[2];" << std::endl;
		buffer << "\tmat4 bone_mat = va_bone_weights[0] * bone_mats[bone_indices[0]];" << std::endl;
		buffer << "\tbone_mat += va_bone_weights[1] * bone_mats[bone_indices[1]];" << std::endl;
		buffer << "\tbone_mat += va_bone_weights[2] * bone_mats[bone_indices[2]];" << std::endl;
		buffer << "\tbone_mat += bone_weight3 * bone_mats[bone_indices[3]];" << std::endl;
		buffer << "\tgl_Position = mvp*bone_mat*vec4(va_position, 1.0);" << std::endl;
	} else {
		buffer << "\tgl_Position = mvp*vec4(va_position, 1.0);" << std::endl;
	}
	buffer << "}" << std::endl;

	// copy to zero terminated string
	size_t len = buffer.str().size();
	char *str = new char[len+1];
	memcpy(str, buffer.str().c_str(), len);
	str[len] = '\0';
	return str;
}

char *generateFragmentShaderSource(MDLVertexFormat *vertex_format /* mat info */) {
	std::stringstream buffer;

	// gl es 2.0 specific
	buffer << "#ifdef GL_ES" << std::endl;
	buffer << "precision mediump float;" << std::endl;
	buffer << "#endif" << std::endl;

	// uniforms
	if (vertex_format->attribs[VAT_TEXCOORD0].components_count) {
		buffer << "uniform sampler2D colormap;" << std::endl;
	}
	buffer << "uniform vec4 u_color;" << std::endl;

	// varyings
	if (vertex_format->attribs[VAT_NORMAL].components_count) {
		buffer << "varying vec3 v_normal;" << std::endl;
	}
	if (vertex_format->attribs[VAT_TEXCOORD0].components_count) {
		buffer << "varying vec2 v_texcoord0;" << std::endl;
	}

	// program
	buffer << "void main() {" << std::endl;
	if (vertex_format->attribs[VAT_NORMAL].components_count) {
		buffer << "\tvec3 light_dir = normalize(vec3(0.0, 0.0, -1.0));" << std::endl;
		buffer << "\tfloat shading = 0.75 + 0.25*dot(normalize(v_normal), -light_dir);" << std::endl;
	} else {
		buffer << "\tfloat shading = 1.0;" << std::endl;
	}
	if (vertex_format->attribs[VAT_TEXCOORD0].components_count) {
		buffer << "\tvec4 color = texture2D(colormap, v_texcoord0);" << std::endl;
	} else {
		buffer << "\tvec4 color = vec4(1.0);" << std::endl;
	}
	buffer << "\tgl_FragColor = vec4(shading * color.rgb * u_color.rgb, color.a * u_color.a);" << std::endl;
	buffer << "}" << std::endl;

	// copy to zero terminated string
	size_t len = buffer.str().size();
	char *str = new char[len+1];
	memcpy(str, buffer.str().c_str(), len);
	str[len] = '\0';
	return str;
}



// MDL file format internals

struct MDLHeader {
	char magic[4];
	int version;
	int file_size;
	int chunk_count;
};

struct MDLChunk {
	int type;
	int size;
};

struct MDLINF1Chunk {
	MDLChunk chunk;
	int node_count;
};
void loadINF1Chunk(MDLModel *model, MDLINF1Chunk *chunk) {
	model->node_count = chunk->node_count;
	model->nodes = new MDLNode[chunk->node_count];

	u8 *data = (u8*)chunk + sizeof(MDLINF1Chunk);
	memcpy(model->nodes, data, (size_t)chunk->node_count*sizeof(MDLNode));
}

struct MDLMAT1Chunk {
	MDLChunk chunk;
	int mat_count;
};
void loadMAT1Chunk(MDLModel *model, MDLMAT1Chunk *chunk) {
	model->texture_count = chunk->mat_count;
	model->textures = new GLuint[chunk->mat_count];

	char *texture_filepaths = (char*)chunk + sizeof(MDLMAT1Chunk);
	for (int ti = 0; ti < chunk->mat_count; ti++) {
		// TODO: don't fetch textures multiple times
		char filepath[256];
		// look in gfx folder
		sprintf(filepath, "data/gfx/%s.tga", texture_filepaths);
		if (access(filepath, R_OK) != -1) { // file exists
			model->textures[ti] = loadTexture2D(filepath, false, nullptr, nullptr);
		} else {
			// might be a compressed texture
			sprintf(filepath, "data/textures/%s", texture_filepaths);
			model->textures[ti] = loadCompressedTexture2D(filepath, nullptr, nullptr);
		}
		texture_filepaths += 64;
	}
}

struct MDLSKL1Chunk {
	MDLChunk chunk;
	int bone_count;
};
void loadSKL1Chunk(MDLModel *model, MDLSKL1Chunk *chunk) {
	model->bone_count = chunk->bone_count;
	model->bones = new MDLBone[chunk->bone_count];
	model->bone_poses = new MDLBoneTransform[chunk->bone_count];
	model->bone_mats = new mat4[chunk->bone_count];

	u8 *data = (u8*)chunk + sizeof(MDLSKL1Chunk);
	memcpy(model->bones, data, (size_t)chunk->bone_count*sizeof(MDLBone));

	// identity pose
	for (int i = 0; i < chunk->bone_count; i++) {
		model->bone_poses[i].translation = v3(0.0f);
		model->bone_poses[i].rotation = quaternion();
		model->bone_poses[i].scale = 1.0f;
	}
}

struct MDLACT1Chunk {
	MDLChunk chunk;
	int action_count;
};
void loadACT1Chunk(MDLModel *model, MDLACT1Chunk *chunk) {
	model->action_count = chunk->action_count;
	model->actions = new MDLAction[chunk->action_count];

	u8 *data = (u8*)chunk;
	int offset = sizeof(MDLACT1Chunk);
	for (int ai = 0; ai < chunk->action_count; ai++) {
		MDLAction *action = model->actions+ai;

		memcpy(action->name, data+offset, 16);
		action->frame_count = *(int*)(data+offset+offsetof(MDLAction, frame_count));
		action->track_count = *(int*)(data+offset+offsetof(MDLAction, track_count));
		offset += 24;

		action->tracks = new MDLActionTrack[action->track_count];
		for (int ti = 0; ti < action->track_count; ti++) {
			MDLActionTrack *track = action->tracks+ti;

			track->bone_index = *(int*)(data+offset);
			offset += sizeof(int);

			track->bone_poses = new MDLBoneTransform[action->frame_count];
			memcpy(track->bone_poses, data+offset, (size_t)action->frame_count*sizeof(MDLBoneTransform));
			offset += (size_t)action->frame_count*sizeof(MDLBoneTransform);
		}

		action->frame = 0; // initial valid frame
	}
}

struct MDLVTX1Chunk {
	MDLChunk chunk;
	int vertex_array_count;
};
void loadVTX1Chunk(MDLModel *model, MDLVTX1Chunk *chunk) {
	model->vertex_array_count = chunk->vertex_array_count;
	model->vertex_arrays = new MDLVertexArray[chunk->vertex_array_count];

	int offset = (int)sizeof(MDLVTX1Chunk);
	for (int vai = 0; vai < chunk->vertex_array_count; vai++) {
		MDLVertexArray *va = model->vertex_arrays+vai;

		u8 *data = (u8*)chunk + offset;
		memcpy(&va->format, data, sizeof(MDLVertexFormat));
		int vertex_count = *(int*)(data+sizeof(MDLVertexFormat));
		size_t vertex_size = va->format.getSize();
		GLsizeiptr vertex_data_size = (GLsizeiptr)vertex_count*(GLsizeiptr)vertex_size;
		u8 *vertex_data = data+sizeof(MDLVertexFormat)+4;
		offset += sizeof(MDLVertexFormat)+4 + vertex_size*(size_t)vertex_count;

		glGenBuffers(1, &va->vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, va->vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

struct MDLTRI1Chunk {
	MDLChunk chunk;
	int mesh_count;
};
void loadTRI1Chunk(MDLModel *model, MDLTRI1Chunk *chunk) {
	model->meshes = new MDLMesh[chunk->mesh_count];
	model->mesh_count = chunk->mesh_count;

	int offset = (int)sizeof(MDLTRI1Chunk);
	for (int mi = 0; mi < chunk->mesh_count; mi++) {
		MDLMesh *mesh = model->meshes+mi;

		char *data = (char*)chunk + offset;
		mesh->vertex_array_index = *(int*)data;
		mesh->index_data_type = *(int*)(data+4);
		mesh->batch_count = *(int*)(data+8);
		mesh->batches = new MDLTriangleBatch[mesh->batch_count];

		offset += 12;

		for (int bi = 0; bi < mesh->batch_count; bi++) {
			MDLTriangleBatch *batch = mesh->batches+bi;

			char *data = (char*)chunk + offset;
			batch->texture_index = *(int*)data;
			batch->index_offset = *(int*)(data+4);
			batch->index_count = *(int*)(data+8);

			offset += 12;
		}
	}
	char *data = (char*)chunk + offset;
	int index_count = *(int*)data;
	offset += 4;
	// unsigned short
	int index_size = 2;
	char *index_data = (char*)chunk + offset;
#if 0
	u16 *indices = (u16*)index_data;
	for (int i = 0; i < index_count; i++)
		printf("%d\n", indices[i]);
#endif

	glGenBuffers(1, &model->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * index_size, index_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



// MDLModel definition

MDLModel::MDLModel() :
	nodes(NULL),
	bones(NULL),
	bone_poses(NULL),
	bone_mats(NULL),
	actions(NULL),
	textures(NULL),
	vertex_arrays(NULL),
	meshes(NULL),
	index_buffer(0) {}

void MDLModel::destroy() {
	for (int i = 0; i < vertex_array_count; i++) {
		if (vertex_arrays[i].vertex_buffer) {
			glDeleteBuffers(1, &vertex_arrays[i].vertex_buffer);
			vertex_arrays[i].vertex_buffer = 0;
		}
	}

	glDeleteTextures(texture_count, textures);
	memset(textures, 0, (size_t)texture_count*sizeof(GLuint));

	if (index_buffer) {
		glDeleteBuffers(1, &index_buffer);
		index_buffer = 0;
	}

	shader.destroy();
}

void MDLModel::free() {
	ARRAY_FREE(nodes);
	node_count = 0;

	ARRAY_FREE(bones);
	ARRAY_FREE(bone_poses);
	ARRAY_FREE(bone_mats)
	bone_count = 0;

	ARRAY_FREE(actions);
	action_count = 0;

	ARRAY_FREE(vertex_arrays);
	vertex_array_count = 0;

	ARRAY_FREE(textures);
	texture_count = 0;

	ARRAY_FREE(meshes);
	mesh_count = 0;
}

void MDLModel::load(const char *filepath) {
	u8 *data = readDataFromFile(filepath, NULL);
	if (!data) return;
	free();

	MDLHeader *header = (MDLHeader*)data;
	if (strncmp(header->magic, "MDL1", 4)) {
		LOGE("%s has wrong magic num\n", filepath);
		delete [] data;
		return;
	}
	if (header->version != 1) {
		LOGE("%s has wrong version\n", filepath);
		delete [] data;
		return;
	}
	//LOGI("filesize: %d\n", header->file_size);

	int offset = (int)sizeof(MDLHeader);
	for (int ci = 0; ci < header->chunk_count; ci++) {
		assert(offset < header->file_size);
		MDLChunk *chunk = (MDLChunk*)(data+offset);
		switch (chunk->type) {
			case 0x31464E49: //magic_to_int("INF1"):
				loadINF1Chunk(this, (MDLINF1Chunk*)chunk);
				break;
			case 0x3154414D: //magic_to_int("MAT1"):
				loadMAT1Chunk(this, (MDLMAT1Chunk*)chunk);
				break;
			case 0x314C4B53: //magic_to_int("SKL1"):
				loadSKL1Chunk(this, (MDLSKL1Chunk*)chunk);
				break;
			case 0x31544341: //magic_to_int("ACT1"):
				loadACT1Chunk(this, (MDLACT1Chunk*)chunk);
				break;
			case 0x31585456: //magic_to_int("VTX1"):
				loadVTX1Chunk(this, (MDLVTX1Chunk*)chunk);
				break;
			case 0x31495254: //magic_to_int("TRI1"):
				loadTRI1Chunk(this, (MDLTRI1Chunk*)chunk);
				break;
			default:
				LOGW("Ignoring unknown chunk %d in %s\n", chunk->type, filepath);
		}
		offset += chunk->size;
	}

	// TODO: move this to loadVTX1Chunk and after that to some shader pool
	char *vert_source = generateVertexShaderSource(&vertex_arrays[0].format); // TODO: multiple vertex_arrays
	char *frag_source = generateFragmentShaderSource(&vertex_arrays[0].format /* mat info */); // TODO: mat info
	//printf("vertex shader src:\n%s\nfragment shader src:\n%s\n", vert_source, frag_source);
	shader.compileAndAttach(GL_VERTEX_SHADER, vert_source);
	shader.compileAndAttach(GL_FRAGMENT_SHADER, frag_source);
	delete [] vert_source;
	delete [] frag_source;
	vertex_arrays[0].format.bindShaderAttribs(&shader);
	shader.link();
	shader.use();
	color_loc = shader.getUniformLocation("u_color");
	glUniform4f(color_loc, 1.0f, 1.0f, 1.0f, 1.0f); // white
	mvp_loc = shader.getUniformLocation("mvp");
	normal_mat_loc = shader.getUniformLocation("normal_mat");
	bone_mats_loc = shader.getUniformLocation("bone_mats");
	colormap_loc = shader.getUniformLocation("colormap");

	delete [] data;
}

int MDLModel::getBoneIndexByName(const char *bone_name) {
	for (int bi = 0; bi < bone_count; bi++) {
		MDLBone *bone = bones+bi;
		if (!strcmp(bone_name, bone->name)) return bi;
	}
	return -1;
}

MDLAction *MDLModel::getActionByName(const char *action_name) {
	for (int ai = 0; ai < action_count; ai++) {
		MDLAction *action = actions+ai;
		if (!strcmp(action_name, action->name)) return action;
	}
	return NULL;
}

void MDLModel::applyAction(MDLAction *action) {
	assert(action->frame >= 0 && action->frame < action->frame_count); // validate frame
	for (int ti = 0; ti < action->track_count; ti++) {
		MDLActionTrack *track = action->tracks+ti;
		bone_poses[track->bone_index] = track->bone_poses[action->frame];
	}
}

void MDLModel::boneMoveTo(int bone_index, vec3 target, vec3 normal) {
	// calc indices of bone chain
	int bone0_index = bone_index;
	assert(bone0_index >= 0 && bone0_index < bone_count);
	int bone1_index = bones[bone0_index].parent_index;
	assert(bone1_index >= 0 && bone1_index < bone_count);
	int bone2_index = bones[bone1_index].parent_index;
	assert(bone2_index >= 0 && bone2_index < bone_count);

	// bone transforms in model space
	MDLBoneTransform bone0_m = calcInheritedBoneTransforms(bone0_index);
	MDLBoneTransform bone1_m = calcInheritedBoneTransforms(bone1_index);
	MDLBoneTransform bone2_m = calcInheritedBoneTransforms(bone2_index);

	vec3 pivot = bone1_m.translation; // center bone of chain is our pivot

	vec3 bone21 = bone1_m.translation - bone2_m.translation; // short hand

	// find angles of triangle from known side lengths using law of cosines
	// cos(gamma) = (a^2 + b^2 - c^2) / (2ab) (gamma is angle in point C)
	// diagram at: https://en.wikipedia.org/wiki/Law_of_cosines
	float l1l1 = lengthSqr(bone21);
	float l2l2 = lengthSqr(target - bone2_m.translation);
	float l3l3 = lengthSqr(bone0_m.translation - bone1_m.translation);
	float cos_gamma_new = (l1l1 + l2l2 - l3l3) / (2.0f*sqrtf(l1l1)*sqrtf(l2l2)); // target angle at bone2

	if (cos_gamma_new >= -1.0f && cos_gamma_new <= 1.0f) { // valid angle => valid solution for kinematic problem
		// all rotations will happen in this plane and around this axis
		vec3 rot_axis_m = normalize(cross(pivot-bone2_m.translation, pivot-target));
		float cos_gamma = dot(normalize(bone21), normalize(target-bone2_m.translation));
		vec3 bone1_new_m = bone2_m.translation + quaternion(rot_axis_m, acosf(cos_gamma_new)-acosf(cos_gamma)) * bone21;
		float cos_beta = dot(normalize(bone1_new_m-bone2_m.translation), normalize(target-bone1_new_m)); // target angle bone1

		vec3 bone2_rot_axis_l = conjugate(bone2_m.rotation) * rot_axis_m;
		vec3 bone1_rot_axis_l = conjugate(bone2_m.rotation) * rot_axis_m;

		// apply result
		bone_poses[bone2_index].rotation = bone_poses[bone2_index].rotation * quaternion(bone2_rot_axis_l, acosf(cos_gamma_new)-acosf(cos_gamma));
		bone_poses[bone1_index].rotation = quaternion(bone1_rot_axis_l, -acosf(cos_beta));

		// rotate bone0 to match normal orientation
		quat init_rot = bone0_m.rotation; // initial orientation of bone0
		quat rot_to_normal = rotateTo(v3(0.0f, 0.0f, 1.0f), normal); // normal is z up by default
		bone_poses[bone0_index].rotation =  conjugate(calcInheritedBoneTransforms(bone1_index).rotation) * rot_to_normal * init_rot;
	}
}

MDLBoneTransform MDLModel::calcInheritedBoneTransforms(int bone_index) {
	// we probably don't want to inherit scale
	MDLBoneTransform bone_pose = bone_poses[bone_index];
	u8 parent_index = bones[bone_index].parent_index;
	while (parent_index != 255) { // parent exists
		bone_pose.rotation = bone_poses[parent_index].rotation * bone_pose.rotation;
		bone_pose.translation = bone_poses[parent_index].translation + bone_poses[parent_index].rotation * bone_pose.translation;
		parent_index = bones[parent_index].parent_index;
	}
	return bone_pose;
}

void MDLModel::calcBoneMats() {
	for (int bi = 0; bi < bone_count; bi++) {
		bone_mats[bi] = calcInheritedBoneTransforms(bi).getMatrix() * bones[bi].inv_bind_pose;
	}
}

void MDLModel::draw(mat4 view_proj_mat) {
	int prev_vertex_format = -1;

	shader.use();
#if 1
	if (bone_mats) {
		calcBoneMats(); // update
		glUniformMatrix4fv(bone_mats_loc, bone_count, GL_FALSE, bone_mats->e);
	}
#endif

	assert(texture_count <= 8);
	for (int ti = 0; ti < texture_count; ti++) {
		glActiveTexture(GL_TEXTURE0+(GLenum)ti);
		glBindTexture(GL_TEXTURE_2D, textures[ti]);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	for (int ni = 0; ni < node_count; ni++) {
		MDLNode *node = nodes+ni;

		mat4 model_mat = m4(node->transform.translation, node->transform.rotation, node->transform.scale);
		mat4 mvp = view_proj_mat*model_mat;
		//mat3 normal_mat = m3(node->transform.rotation);
		glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, mvp.e);
		//glUniformMatrix3fv(renderer.normal_mat_loc, 1, GL_FALSE, normal_mat.e);

		// TODO: frustum culling

		MDLMesh *mesh = meshes+node->mesh_index;

		if (mesh->vertex_array_index != prev_vertex_format) {
			if (prev_vertex_format != -1) {
				MDLVertexArray *vertex_array = vertex_arrays+prev_vertex_format;
				vertex_array->format.disable();
			}
			prev_vertex_format = mesh->vertex_array_index;

			MDLVertexArray *vertex_array = vertex_arrays+mesh->vertex_array_index;

			glBindBuffer(GL_ARRAY_BUFFER, vertex_array->vertex_buffer);
			// enable vertex format
			vertex_array->format.enable();
		}

		for (int bi = 0; bi < mesh->batch_count; bi++) {
			MDLTriangleBatch *batch = mesh->batches+bi;

			glUniform1i(colormap_loc, batch->texture_index);
			// TODO: variable index type
			glDrawElements(GL_TRIANGLES, batch->index_count, GL_UNSIGNED_SHORT, (GLvoid*)((size_t)batch->index_offset*sizeof(u16)));
		}
	}

	if (prev_vertex_format != -1) {
		MDLVertexArray *vertex_array = vertex_arrays+prev_vertex_format;
		vertex_array->format.disable();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}
