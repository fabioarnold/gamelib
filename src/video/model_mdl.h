enum MDLDataType {
	DT_FLOAT = 0,
	DT_UNSIGNED_BYTE,
	DT_UNSIGNED_SHORT,
	DT_UNSIGNED_INT,

	DT_COUNT
};
// conversion tables
static const GLenum dtGLTypes[DT_COUNT] = {
	GL_FLOAT,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT
};
static const size_t dtSizes[DT_COUNT] = {
	sizeof(float),
	sizeof(unsigned char),
	sizeof(unsigned short),
	sizeof(unsigned int)
};

enum MDLVertexAttribType {
	VAT_POSITION = 0,
	VAT_NORMAL,
	VAT_TANGENT,
	VAT_TEXCOORD0,
	VAT_TEXCOORD1,
	VAT_COLOR,
	VAT_BONE_INDEX,
	VAT_BONE_WEIGHT,

	VAT_COUNT // 8
};

struct MDLVertexAttrib {
	u8 components_count;
	u8 /* DataType */ data_type;
};

struct MDLVertexFormat {
	MDLVertexAttrib attribs[VAT_COUNT]; // indexed by MDLVertexAttribtype

	size_t getSize(); // in bytes
	size_t getAttribOffset(GLuint attribType); // in bytes

	void enable(void *base_pointer=NULL);
	void disable();

	void bindShaderAttribs(Shader *shader); // bind active attribs to shader
};

struct MDLVertexArray {
	MDLVertexFormat format; // TODO: generate shader for vertex format?
	GLuint vertex_buffer;
};

struct MDLTriangleBatch {
	int texture_index; // material index
	int index_offset;
	int index_count;
};

struct MDLMesh {
	int vertex_array_index;
	int /* DataType */ index_data_type; // BYTE, SHORT or INT
	//GLuint index_buffer;

	int batch_count;
	MDLTriangleBatch *batches;
};

struct MDLAABB {
	vec3 min;
	vec3 max;
};

struct MDLNode { // TODO: add name
	int mesh_index;
	Transform transform;
	MDLAABB aabb;
};

struct MDLBone {
	char name[15];
	u8 parent_index; // 255 -> no parent
	mat4 inv_bind_pose;
};

struct MDLBoneTransform { // in order of application
	vec3 translation;
	quat rotation;
	float scale;

	mat4 getMatrix();
};

struct MDLActionTrack {
	int bone_index;
	MDLBoneTransform *bone_poses;
};

struct MDLAction {
	char name[16];
	int frame_count;

	int track_count;
	MDLActionTrack *tracks;

	// playback
	int frame;
};

struct MDLModel {
	int node_count;
	MDLNode *nodes;

	int bone_count;
	MDLBone *bones;
	MDLBoneTransform *bone_poses; // the current pose
	mat4 *bone_mats;

	int action_count;
	MDLAction *actions;

	int texture_count;
	GLuint *textures; // opengl names

	int vertex_array_count;
	MDLVertexArray *vertex_arrays;

	int mesh_count;
	MDLMesh *meshes;

	GLuint index_buffer;
	Shader shader; // TODO: make this a reference (also multiple shader per vertex array/material)
	// cached uniform locations
	GLint mvp_loc;
	GLint normal_mat_loc;
	GLint bone_mats_loc;
	GLint colormap_loc;

	MDLModel();
	void load(const char *filepath); // TODO: switch to memory arena
	void free();

	int getBoneIndexByName(const char *bone_name); // returns -1 when bone could not be found

	MDLAction *getActionByName(const char *action_name);
	void applyAction(MDLAction *action); // replaces poses
	void blendAction(MDLAction *action, float weight); // mix action

	// using analytical inverse kinematics solution with two chain links
	// the bone's parent will be used as pivot for the orientation of the bend
	// bone should have 2 parents
	// target_location and target_normal have to be provided in model space
	// overwrites bone_poses, so apply actions first
	void boneMoveTo(int bone_index, vec3 target_location, vec3 target_normal);

	void draw(mat4 view_proj_mat);

	// private
	MDLBoneTransform calcInheritedBoneTransforms(int bone_index);
	void calcBoneMats(); // converts bone tranforms to mats
};
