CC = cc
LEX = flex

SOURCE = default
TARGET = default
INPUT = default
C_FILE = lex.yy.c

.PHONY: all
all: ${SOURCE}
	@${LEX} ${SOURCE}
	@${CC} -o ${TARGET} ${C_FILE} -ll
	@./${TARGET} ${INPUT}
	@rm ${C_FILE}
