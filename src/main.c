#include <stdio.h>

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

int main(int argc, char* argv[]) {
	for (int i = 0; i < argc; i++) {
		printf("strlen(\"%s\"): %u\n", argv[i], (unsigned int)strlen(argv[i]));
	}

	if (argc == 1) {
		// Attempt to read from stdin
		// Else error and return
	} else {
		for (int i = 1; i < argc; i++) {
			char* arg = argv[i];
			size_t len = strlen(arg);
			
			if (arg[0] == '-') {
				if (arg[1] == '-') {
				// Handle '--word' options
				// --bytes
				// --chars
				// --lines
				// --max-line-length
				// --words
				} else if (len == 2){
				// Handle '-x' options	
					switch(arg[1]) {
						case 'c':
						case 'm':
						case 'l':
						case 'L':
						case 'w':
						default:
							//Invalid option
							break;
					}
				} else {
					// Invalid option
				}
			}
		}
	}

	return 0;
}
