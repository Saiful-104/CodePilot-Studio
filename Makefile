.PHONY: all compiler clean

all: compiler

compiler:
	$(MAKE) -C compiler

clean:
	$(MAKE) -C compiler clean
