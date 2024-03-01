#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define array_len(arr) sizeof(arr) / sizeof(arr[0])

typedef enum WC_Print_Options {
	OPTION_BYTE_COUNT = 1 << 0,	// -c, --bytes
	OPTION_CHAR_COUNT = 1 << 1,	// -m, --chars
	OPTION_LINE_COUNT = 1 << 2,	// -l, --lines
	OPTION_WORD_COUNT = 1 << 3,	// -w, --words
	OPTION_LINE_LENGTH = 1<< 4	// -L, --max-line-length
} WC_Print_Options;

typedef struct WC_Results {
	uint64_t bc, cc, lc, wc, ml;
} WC_Results;

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

int strcmp(const char *lhs, const char *rhs) {
	unsigned char lc, rc;
	int i = 0;

	do {
		lc = lhs[i];
		rc = rhs[i];
		i++;
	} while (lc == rc && lc != '\0');

		
	return lc-rc;
}

int isspace(int ch) {
	return (int)(
		ch == ' ' 	||
		ch == '\f'	||
		ch == '\n'	||
		ch == '\r'	||
		ch == '\t'	||
		ch == '\v'
	);	
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

// TODO: Support for multi-byte characters
WC_Results wc2(char* buf) {
	WC_Results result = {};
	uint64_t word_length = 0;
	uint64_t line_length = 0;
	int i = 0, c = 0; // u, p

	while(1) {
		c = buf[i++];
		if (c == '\0') {
			break;
		} else if (isspace(c)) {
			// TODO: Handle goofy shit like \r\n line endings?
			if (c == '\n') {
				if (line_length > result.ml) {
					result.ml = line_length;
				}
				result.lc++;
				line_length = 0;
			}

			if (word_length > 0) {
				result.wc++;
			}
			word_length = 0;
		} else {
			word_length++;
			line_length++;
		}

	}

	result.bc = i+1;
	result.cc = i+1;

	return result;
}

void print_wc2(WC_Results results, WC_Print_Options options) {
	if (options & OPTION_LINE_COUNT) {
		printf(" %zu", results.lc);
	}
	if (options & OPTION_WORD_COUNT) {
		printf(" %zu", results.wc);
	}
	// NOTE: Currently incorrect, likely due to multi-byte characters
	if (options & OPTION_CHAR_COUNT) {
		printf(" %zu", results.cc);
	}
	if (options & OPTION_BYTE_COUNT) {
		printf(" %zu", results.bc);
	}
	if (options & OPTION_LINE_LENGTH) {
		printf(" %zu", results.ml);
	}
}

int main(int argc, char* argv[]) {
//	for (int i = 0; i < argc; i++) {
//		printf("strlen(\"%s\"): %u\n", argv[i], (unsigned int)strlen(argv[i]));
//	}

	WC_Print_Options options = 0;	

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
					// TODO: Handle '--' option
					if (strcmp(arg+2, "lines") == 0) {
						options |= OPTION_LINE_COUNT;
					}
					else if (strcmp(arg+2, "words") == 0) {
						options |= OPTION_WORD_COUNT;
					}
					else if (strcmp(arg+2, "chars") == 0) {
						options |= OPTION_CHAR_COUNT;
					}
					else if (strcmp(arg+2, "bytes") == 0) {
						options |= OPTION_BYTE_COUNT;
					}
					else if (strcmp(arg+2, "max-line-length") == 0) {
						options |= OPTION_LINE_LENGTH;
					}
					else {
						printf("wc2: invalid option: %s\n", arg);
						printf("Try 'wc2 --help' for more information.\n");
						exit(1);
					}
				} else if (len == 2) {
					// Handle '-x' options
					switch(arg[1]) {
						case 'l':
							options |= OPTION_LINE_COUNT;
							break;
						case 'w':
							options |= OPTION_WORD_COUNT;
							break;
						case 'm':
							options |= OPTION_CHAR_COUNT;
							break;
						case 'c':
							options |= OPTION_BYTE_COUNT;
							break;
						case 'L':
							options |= OPTION_LINE_LENGTH;
							break;
						default:
							printf("wc2: invalid option: %s\n", arg);
							printf("Try 'wc2 --help' for more information.\n");
							exit(1);
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

	if (options == 0) {
		options = OPTION_LINE_COUNT | OPTION_WORD_COUNT | OPTION_BYTE_COUNT;
	}

	if (fn_len == 0) {
		size_t len = 0;
		char* buf = fread_all(stdin, &len);
		if (len > 0) {
			WC_Results results = wc2(buf);
			results.bc = len;
			print_wc2(results, options);
			printf("\n");
			free(buf);
		}
	}

	for (int i = 0; i < fn_len; i++) {
		FILE* fstream = fopen(file_names[i], "r");
		if (fstream != NULL) {
			size_t len = 0;
			char* buf = fread_all(fstream, &len);

			if (len > 0) {
				WC_Results counts = wc2(buf);
				counts.bc = len;
				print_wc2(counts, options);
				free(buf);
			}
			printf(" %s\n", file_names[i]);
			fclose(fstream);
		} else {
			printf("%s: No such file or directory\n", file_names[i]);			
		}
	}

	return 0;
}
