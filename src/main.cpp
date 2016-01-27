#include <stdio.h>
#include <iostream>
#include "jit/jfunction.h"
#include "utils/bytebuffer.h"
#include "parser/parser.h"

int writeToFile(Assembler::ByteBuffer const& buffer, char const* filename) {
	
	FILE* fout = fopen(filename, "wb");
	
	if (!fout) {
		printf("Could not open %s to write\n", filename);
		return -1;
	}

	if (buffer.current()) {
		if (fwrite(buffer.raw(), buffer.current(), 1, fout) != 1) {
			printf("Could not write to file %s\n", filename);
			fclose(fout);
			return -1;
		}
	}

	fclose(fout);
	
	return 0;
}

char* readFromFile(char const* filename) {
	FILE* fin = fopen(filename, "rb");

	if (!fin) {
		printf("Could not open file %s\n", filename);
		return 0;
	}

	fseek(fin, 0, SEEK_END);
	unsigned long size = ftell(fin);
	fseek(fin, 0, SEEK_SET);

	char* buffer = new char[size + 1];
	if (fread(buffer, size, 1, fin) != 1) {
		//Clear the buffer and return 0 if the load fails
		printf("Failed to read the file %s into memory\n", filename);
		delete[] buffer;
		fclose(fin);
		return 0;
	}

	fclose(fin);
	buffer[size] = 0;
	return buffer;
}

bool isFlag(char const* name, int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], name) == 0) {
			return true;
		}
	}
	return false;
}

char* getArgument(char const* name, int argc, char** argv) {
	
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], name) == 0) {
			return i < (argc - 1) ? argv[i+1] : nullptr;
		}
	}

	return nullptr;
}

int doInteractive(Assembler::Parser& parser) {

	printf("Entering interactive mode\n");
	char buffer[4096];

	while(true) {
		printf("Query: ");
		std::cin.getline(buffer, 4096);
		parser.parse(buffer);
	}
}

int doFile(char const* targetFile, Assembler::Parser& parser) {
	char* inputSource = readFromFile(targetFile);

	if (!inputSource) {
		printf("Could not load input source\n");
		return -1;
	}

	if (!parser.parse(inputSource)) {
		return -1;
	}

	delete[] inputSource;

	return 0;
}

char const* findTarget(int argc, char** argv) {
	return argc > 1 ? argv[argc - 1] : nullptr;
}

int main(int argc, char** argv) {

	printf("ATS - Annotated Type System\n");

	if (isFlag("-h", argc, argv) || isFlag("--help", argc, argv)) {
		printf("Run with no arguments to enter interactive mode\n");
		printf("Run %s ./scriptpath to execute a script\n", argv[0]);
		printf("Run %s -i ./scriptpath to execute a script interactively", argv[0]);
		return 0;
	}

	bool interactive = isFlag("-i", argc, argv) || isFlag("--interactive", argc, argv);
	char const* targetFile = findTarget(argc, argv);

	Assembler::Parser parser;

	if (targetFile) {
		if (doFile(targetFile, parser) != 0) {
			return -1;
		}
	} else {
		interactive = true;
	}

	if (interactive) {
		return doInteractive(parser);
	} else {
		return 0;
	}
}
