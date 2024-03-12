#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define array_len(arr) sizeof(arr) / sizeof(arr[0])

#define da_push(arr, val, len, cap) \
if (len >= cap) { \
	cap *= 2; \
	void* new_arr = realloc(arr, sizeof(arr[0]) * cap); \
	if (new_arr) { arr = new_arr; } \
} \
arr[len++] = val

typedef enum WC_Print_Options {
	OPTION_BYTE_COUNT  = 1 << 0,	// -c, --bytes
	OPTION_CHAR_COUNT  = 1 << 1,	// -m, --chars
	OPTION_LINE_COUNT  = 1 << 2,	// -l, --lines
	OPTION_WORD_COUNT  = 1 << 3,	// -w, --words
	OPTION_LINE_LENGTH = 1 << 4	// -L, --max-line-length
} WC_Print_Options;

typedef struct WC_Results {
	uint64_t bc, cc, lc, wc, ml;
} WC_Results;

// Count the number of digits in an unsigned integer
size_t uintlen(uint64_t val) {
	size_t result = 0;

	do {
		result++;
	}
	while((val /= 10));

	return result;
}

size_t strlen(const char* str) {
	if (str == 0 || str[0] == '\0') {
		return 0;
	}
	size_t result = 0;
	char c = 0;
	do {
		c = str[result++];
	} while (c != '\0');
	
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

		
	return (int)lc-(int)rc;
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

int file_exists(const char* fn) {
	FILE* stream = fopen(fn, "r");
	if (stream != NULL) {
		fclose(stream);
		return 1;
	}

	return 0;
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

WC_Results wc2_buf(const char* buf) {
	WC_Results result = {};
	uint64_t word_length = 0;
	uint64_t line_length = 0;

	while(*buf != '\0') {
		if (isspace(*buf)) {
			if (*buf == '\n') {
				if (line_length > result.ml) {
					result.ml = line_length;
				}
				line_length = 0;
				result.lc++;
			} else { line_length++; }

			if (word_length > 0) {
				result.wc++;
			}
			word_length = 0;
		} else {
			word_length++;
			line_length++;
		}
		
		// Most significant bit of a single byte character is 0
		// Two most significant bits of the first byte of a multibyte character are 11
		// Two most significant bits of subsequent bytes in a multibyte character are 10
		if ((*buf & 0xC0) != 0x80) { result.cc++; }
		result.bc++;

		buf++;
	}

	return result;
}

WC_Results wc2_fstream(FILE* stream) {
	char buf[1024];
	WC_Results result = {};
	uint64_t word_length = 0, line_length = 0, bytes_read = 0;
	
	while (bytes_read = fread(buf, sizeof(char), array_len(buf), stream)) {
		for (int i = 0, c = 0; i < bytes_read; i++){
			c = buf[i];

			if (isspace(c)) {
				if (c == '\n') {
					if (line_length > result.ml) {
						result.ml = line_length;
					}
					line_length = 0;
					result.lc++;
				} else { line_length++; }

				if (word_length > 0) {
					result.wc++;
				}
				word_length = 0;
			} else {
				word_length++;
				line_length++;
			}
			
			// Most significant bit of a single byte character is 0
			// Two most significant bits of the first byte of a multibyte character are 11
			// Two most significant bits of subsequent bytes in a multibyte character are 10
			if ((c & 0xC0) != 0x80) { result.cc++; }
		}

		result.bc += bytes_read;
	}


	return result;
}
static inline WC_Results wc2_max_cols(WC_Results results, WC_Results cols) {
	uint64_t col = 0;
	if ( cols.bc < (col = uintlen(results.bc))) {
		cols.bc = col;
	}
	if ( cols.cc < (col = uintlen(results.cc))) {
		cols.cc = col;
	}
	if ( cols.lc < (col = uintlen(results.lc))) {
		cols.lc = col;
	}
	if ( cols.wc < (col = uintlen(results.wc))) {
		cols.wc = col;
	}
	if ( cols.ml < (col = uintlen(results.ml))) {
		cols.ml = col;
	}

	return cols;
}

void sprint_wc2_field(char* buf, uint64_t field, uint64_t width) {
	int64_t padding = (int64_t)width - (int64_t)uintlen(field);
	if (padding < 0) padding = 0;
	for (int i = 0; i < padding; i++) {
		buf[i] = ' ';
	}
	sprintf(buf+padding, "%zu", field);
}

void print_wc2(WC_Results results, WC_Results cols, WC_Print_Options options) {
	size_t widest = 0;
	if (widest < cols.lc) widest = cols.lc;
	if (widest < cols.wc) widest = cols.wc;
	if (widest < cols.cc) widest = cols.cc;
	if (widest < cols.bc) widest = cols.bc;
	if (widest < cols.ml) widest = cols.ml;

	size_t buflen = sizeof(char) * widest * 2;
	char* buf = malloc(buflen);
	if (!buf) {
		fprintf(stderr, "print_wc2(): Failed to allocate %zu bytes\n", buflen);
		exit(1);
	}
	
	uint64_t not_first = 0;
	if (options & OPTION_LINE_COUNT) {
		sprint_wc2_field(buf, results.lc, cols.lc+not_first);
		printf("%s", buf);
		not_first = 1;
	}
	if (options & OPTION_WORD_COUNT) {
		sprint_wc2_field(buf, results.wc, cols.wc+not_first);
		printf("%s", buf);
		not_first = 1;
	}
	if (options & OPTION_CHAR_COUNT) {
		sprint_wc2_field(buf, results.cc, cols.cc+not_first);
		printf("%s", buf);
		not_first = 1;
	}
	if (options & OPTION_BYTE_COUNT) {
		sprint_wc2_field(buf, results.bc, cols.bc+not_first);
		printf("%s", buf);
		not_first = 1;
	}
	if (options & OPTION_LINE_LENGTH) {
		sprint_wc2_field(buf, results.ml, cols.ml+not_first);
		printf("%s", buf);
		not_first = 1;
	}

	free(buf);
}

static inline WC_Results wc2_sum(WC_Results first, WC_Results second) {
	return (WC_Results){
		.bc = first.bc + second.bc,
		.cc = first.cc + second.cc,
		.lc = first.lc + second.lc,
		.wc = first.wc + second.wc,
		.ml = first.ml > second.ml ? first.ml : second.ml
	};
}

int main(int argc, char* argv[]) {
	WC_Print_Options options = 0;	

	size_t fn_cap = 8;
	size_t fn_len = 0;
	char** file_names = malloc(sizeof(char*) * fn_cap);

	int end_options = 0;

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		
		if (arg[0] == '-' && !end_options) {
			if (arg[1] == '-') { // Handle -- options
				if (arg[2] == '\0') {
					end_options = 1;
				}
				else if (strcmp(arg+2, "lines") == 0) {
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
				else if (strcmp(arg+2, "files0-from") == '=' && (arg+2)[strlen("files0-from")] == '=') {
					char* f0f_filename = arg+strlen("--files0-from=");
					FILE* f0f_stream = 0;
					if (f0f_filename[0] == '-' && f0f_filename[1] == '\0') {
						f0f_stream = stdin;	
					} else {
						f0f_stream = fopen(f0f_filename, "r");
					}

					if (f0f_stream != NULL) {
						size_t len = 0;
						char* f0f_list = fread_all(f0f_stream, &len);

						da_push(file_names, f0f_list, fn_len, fn_cap);
						for (int i = 0; i < len-1; i++) {
							if (f0f_list[i] == '\0') {
								char* new_fn = f0f_list+i+1;
								da_push(file_names, f0f_list+i+1, fn_len, fn_cap);
									
							}
						}
						fclose(f0f_stream);
					} else {
						fprintf(stderr, "%.*s: No such file or directory\n", 256, file_names[i]);			
					}
				}
				else {
					fprintf(stderr, "wc2: invalid option: %s\n", arg);
					fprintf(stderr, "Try 'wc --help' for more information.\n");
					exit(1);
				}
			} else { // Handle - options
				for (int i = 1; arg[i] != '\0'; i++) {
					switch(arg[i]) {
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
							fprintf(stderr, "wc2: invalid option: %s\n", arg);
							fprintf(stderr, "Try 'wc --help' for more information.\n");
							exit(1);
							break;
					}
				}
			} 
		} else {
			da_push(file_names, arg, fn_len, fn_cap);
		}
	}

	if (options == 0) {
		options = OPTION_LINE_COUNT | OPTION_WORD_COUNT | OPTION_BYTE_COUNT;
	}

	WC_Results* results = calloc(fn_len+1, sizeof(WC_Results));
	WC_Results total = {}, cols = {};

	for (int i = 0; i < fn_len; i++) {
		FILE* fstream = fopen(file_names[i], "r");
		if (fstream != NULL) {
			results[i] = wc2_fstream(fstream);
			cols = wc2_max_cols(results[i], cols);
			total = wc2_sum(total, results[i]);
			fclose(fstream);
		} else {
			fprintf(stderr, "wc2: %.*s: No such file or directory\n", 256, file_names[i]);
		}
	}

	if (fn_len == 0) {
		results[0] = wc2_fstream(stdin);
		if (results[0].bc > 0) {
			cols = (WC_Results){7,7,7,7,7};
			cols = wc2_max_cols(results[0], cols);

			file_names[0] = "";
			fn_len++;
		}
	}


	if (fn_len >= 2) {
		cols = wc2_max_cols(total, cols);

		for (int i = 0; i < fn_len; i++) {
			printf(" ");
			print_wc2(results[i], cols, options);
			printf(" %s\n", file_names[i]);
		}
		printf(" ");
		print_wc2(total, cols, options);
		printf(" total\n");
	} else if (results[0].bc) {
		uint64_t cm = cols.bc;
		cm = cm > cols.cc ? cm : cols.cc;
		cm = cm > cols.lc ? cm : cols.lc;
		cm = cm > cols.wc ? cm : cols.wc;
		cm = cm > cols.ml ? cm : cols.ml;

		print_wc2(results[0], (WC_Results){cm,cm,cm,cm,cm}, options);
		if (file_names[0][0] != '\0') {
			printf(" %s\n", file_names[0]);
		}
	}

	return 0;
}
