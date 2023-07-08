CLASSDIR = ../cool/cool

FILES = $(wildcard *.cl)
TARGET = temp.s

all: source test clean

source: ${FILES}
	@echo "\e[32;1mCompiling...\e[33;0;m"
	@${CLASSDIR}/bin/coolc ${FILES} -o ${TARGET}
	@echo "\e[32;1mCompile OK!\e[33;0;m"

test: ${TARGET}
	@echo "\e[32;1mStart testing...\e[33;0m"
	@${CLASSDIR}/bin/spim -trap_file ${CLASSDIR}/lib/trap.handler -file ${TARGET}
	@echo "\e[32;1mTest finished!\e[33;0m"

clean:
	@rm ${TARGET}
	@echo "\e[32;1mClean finished!\e[33;0m"