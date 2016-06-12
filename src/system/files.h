size_t getFileSize(FILE *opened_file);

u8 *readDataFromFile(const char *filepath, size_t *out_data_size=NULL);
char *readStringFromFile(const char *filepath, int *str_len=NULL);
