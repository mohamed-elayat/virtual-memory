BUILD_DIR=build
CC=gcc
CFLAGS=-g -std=gnu99 -Wall -pedantic -Isrc -I$(BUILD_DIR)
LDFLAGS=
BISON=bison
FLEX=flex

LOGS=logs

OS := $(shell uname)
ifeq ($(OS),Darwin)
TAR=gtar # gtar for MAC (brew install gnu-tar) for linux change to tar
else
TAR=tar
endif

OBJS = tlb.o parse.tab.o vmm.o pm.o pt.o tokens.o common.o

.PHONY: default all clean format client server test

default: all

all: $(BUILD_DIR)/vmm #rapport.pdf

$(BUILD_DIR)/vmm: $(patsubst %.o, $(BUILD_DIR)/%.o, $(OBJS))
	$(CC) $(LDFLAGS) -o $@ $(patsubst %.o,$(BUILD_DIR)/%.o, $(OBJS))

$(BUILD_DIR)/%.o: src/%.c
	@[ -d "$$(dirname "$@")" ] || mkdir -p "$$(dirname "$@")"
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/parse.tab.c $(BUILD_DIR)/parse.tab.h: src/parse.y
	@[ -d "$$(dirname "$@")" ] || mkdir -p "$$(dirname "$@")"
	cd $(BUILD_DIR); $(BISON) -d ../src/parse.y

$(BUILD_DIR)/tokens.c: src/tokens.l
	@[ -d "$$(dirname "$@")" ] || mkdir -p "$$(dirname "$@")"
	$(FLEX) -o $@ $<

run: all
	$(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command.in

run1: all
	$(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command1.in

run2: all
	$(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command2.in


run3: all
	$(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command3.in

run4: all
	$(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command4.in



valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command.in

valgrind1: all
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command1.in

valgrind2: all
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command2.in

valgrind3: all
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command3.in

valgrind4: all
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/vmm -p logs/pm_log.txt -t logs/tlb_log.txt -l logs/pt_log.txt -c logs/vmm_log.txt tests/BACKING_STORE.txt < tests/command4.in

clean:
	$(RM) -r $(BUILD_DIR) $(LOGS)/*.txt *.aux *.log

%.pdf: %.tex
	pdflatex $<

release:
	$(TAR) -czv -f tp3.tar.gz --transform 's|^|tp3/|' \
	    src *.tex *.pdf tests GNUmakefile
