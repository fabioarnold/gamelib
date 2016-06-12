size_t getFileSize(FILE *opened_file) {
	fseek(opened_file, 0, SEEK_END);
	size_t file_size = ftell(opened_file);
	fseek(opened_file, 0, SEEK_SET);

	return file_size;
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

char *readStringFromFile(const char *filepath, int *str_len) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		LOGE("Couldn't read %s", filepath);
		return NULL;
	}

	size_t file_size = getFileSize(file);
	char *str = new char[file_size + 1];
	fread(str, sizeof(char), file_size, file);
	str[file_size] = '\0'; // zero terminated string
	fclose(file);

	if (str_len) {
		*str_len = (int)file_size-1;
	}

	return str;
}
