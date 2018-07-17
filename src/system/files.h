// TODO rename to start with file-prefix or use namespace

size_t getFileSize(FILE *opened_file);

bool doesFileExist(const char *filepath);

u8 *readDataFromFile(const char *filepath, size_t *out_data_size=NULL);
char *readStringFromFile(const char *filepath, size_t *str_len=NULL);

bool writeDataToFile(const char *filepath, u8 *data, size_t data_size);
bool writeStringToFile(const char *filepath, char *str, size_t str_len=0);
