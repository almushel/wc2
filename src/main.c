#include <stdio.h>
#include <stdlib.h>

#define array_len(arr) sizeof(arr) / sizeof(arr[0])

typedef enum WC_Print_Options {
	OPTION_BYTE_COUNT = 1,	// -c, --bytes
	OPTION_CHAR_COUNT,	// -m, --chars
	OPTION_LINE_COUNT,	// -l, --lines
	OPTION_WORD_COUNT,	// -w, --words
	OPTION_LINE_LENGTH	// -L, --max-line-length
} WC_Print_Options;

long unsigned int strlen(const char* s) {
	if (s[0] == '\0') {
		return 0;
	}

	long unsigned int result = 0;
	int i = 0;
	char c = 0;
	
	do {
		result++;

		i++;
		c = s[i];
	} while(c != '\0');

	return result;
}

char* fread_all(FILE* stream, size_t* size) {
	size_t cap = sizeof(char) * 1024;
	char* result = malloc(cap);
	size_t read = 0;

	*size = 0;
	while (read = fread(result + *size, sizeof(char), 1024, stream)) {
		*size += read;	
		if (*size >= cap) {
			cap *= 2;
			char* r = realloc(result, cap);
			if (r != NULL) {
				result = r;
			}
		}
	}

	char* r = realloc(result, *size);
	if (r != NULL) {
		result = r;
	}

	return result;
}

int main(int argc, char* argv[]) {
//	for (int i = 0; i < argc; i++) {
//		printf("strlen(\"%s\"): %u\n", argv[i], (unsigned int)strlen(argv[i]));
//	}

	WC_Print_Options options;	

	size_t fn_cap = 8;
	size_t fn_len = 0;
	char** file_names = malloc(sizeof(char*) * fn_cap);

	if (argc == 1) {
		// Attempt to read from stdin
		// Else error and return
	} else {
		for (int i = 1; i < argc; i++) {
			char* arg = argv[i];
			size_t len = strlen(arg);

			// printf("%s", arg);
			
			if (arg[0] == '-') {
				if (arg[1] == '-') {
				// Handle '--word' options
				// --bytes
				// --chars
				// --lines
				// --max-line-length
				// --words
				} else if (len == 2) {
				// Handle '-x' options	
					switch(arg[1]) {
						case 'c':
							options |= OPTION_BYTE_COUNT;
							break;
						case 'm':
							options |= OPTION_CHAR_COUNT;
							break;
						case 'l':
							options |= OPTION_LINE_COUNT;
							break;
						case 'L':
							options |= OPTION_LINE_LENGTH;
							break;
						case 'w':
							options |= OPTION_WORD_COUNT;
							break;
						default:
							//Invalid option
							break;
					}
				} 
			} else {
				if(fn_len >= fn_cap) {
					fn_cap *= 2;
					char** fn_new = realloc(file_names, sizeof(char*) * fn_cap);
					if (fn_new) {
						file_names = fn_new;
					}
				}
				file_names[fn_len++] = arg;
			}
		}
	}

	for (int i = 0; i < fn_len; i++) {
		printf("wc2: ");
		FILE* fstream = fopen(file_names[i], "r");
		if (fstream != NULL) {
			size_t len = 0;
			char* buf = fread_all(fstream, &len);
			if (len > 0) {
				if (options & OPTION_LINE_COUNT) {}
				if (options & OPTION_WORD_COUNT) {}
				if (options & OPTION_CHAR_COUNT) {}
				if (options & OPTION_BYTE_COUNT) {
					printf("%zu ", len);
				}
				if (options & OPTION_LINE_LENGTH) {}

				free(buf);
			}
			printf("%s\n", file_names[i]);
			fclose(fstream);
		} else {
			printf("%s: No such file or directory\n", file_names[i]);			
		}

	}

	return 0;
}
