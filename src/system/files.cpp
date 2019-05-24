size_t getFileSize(FILE *opened_file) {
	size_t file_size = 0;
	fseek(opened_file, 0, SEEK_END);
	long res = ftell(opened_file);
	if (res < 0) {
		LOGE("Couldn't get file size");
	} else {
		file_size = (size_t)res;
	}
	fseek(opened_file, 0, SEEK_SET);

	return file_size;
}

bool doesFileExist(const char *filepath) {
	// there's access and stat, but windows requires special treatment...
	// so do it the dumb and inefficient way
	FILE *file = fopen(filepath, "rb");
	bool exists = !!file;
	if (exists) fclose(file);
	return exists;
}

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
bool doesDirExist(const char *path) {
	struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

void createDir(const char *path) {
#ifdef _WIN32
	_mkdir(path);
#else
	mkdir(path, 0755);
#endif
}

u8 *readDataFromFile(const char *filepath, size_t *out_data_size) {
	FILE *file = fopen(filepath, "rb");
	if (!file) {
		LOGE("Couldn't read %s", filepath);
		return NULL;
	}

	size_t file_size = getFileSize(file);
	u8 *data = new u8[file_size];
	fread(data, sizeof(u8), file_size, file);
	fclose(file);

	if (out_data_size) {
		*out_data_size = file_size;
	}

	return data;
}

char *readStringFromFile(const char *filepath, size_t *str_len) {
	FILE *file = fopen(filepath, "r"); // r means textmode: drops \r on windows so file_size != str_len
	if (!file) {
		LOGE("Couldn't read %s", filepath);
		return NULL;
	}

	size_t file_size = getFileSize(file);
	char *str = new char[file_size + 1]; // we need at least this much storage
	size_t bytes_read = fread(str, sizeof(char), file_size, file);
	assert(bytes_read <= file_size);
	str[bytes_read] = '\0'; // zero terminated string
	fclose(file);

	if (str_len) *str_len = bytes_read;

	return str;
}

bool writeDataToFile(const char *filepath, u8 *data, size_t data_size) {
	FILE *file = fopen(filepath, "wb");
	if (!file) {
		LOGE("Couldn't read %s", filepath);
		return false;
	}

	fwrite(data, sizeof(u8), data_size, file);
	fclose(file);

	return true;
}

// returns true if successful
// if str_len is 0 str is assumed to be 0 terminated and str_len will be calculated
bool writeStringToFile(const char *filepath, char *str, size_t str_len) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		LOGE("Couldn't read %s", filepath);
		return false;
	}
	if (!str_len) str_len = strlen(str);

	fwrite(str, sizeof(char), str_len, file);
	char zero = 0;
	fwrite(&zero, sizeof(char), 1, file);
	fclose(file);

	return true;
}
