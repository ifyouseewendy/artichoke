MUSL = $(shell ls musl/*.c | sed -e's/\.c/.o/')
OBJ = $(shell ls src/*.c | sed -e's/\.c/.o/') \
	$(shell find build/sys/ -type f -name '*.c' | sed -e's/\.c/.o/') \
	$(shell find mrbgems/mruby-compiler/ -type f -name '*.c' | sed -e's/\.c/.o/') \
	entry.o
OUTPUT = library.wasm

COMPILE_FLAGS = -DMRB_DISABLE_STDIO -DMRB_UTF8_STRING -DMRB_DISABLE_DIRECT_THREADING -DMRB_INT32 \
		-O3 \
		--target=wasm32-unknown-unknown \
		-flto \
		-fno-builtin-malloc -fno-builtin-realloc -nostdlib \
		-fvisibility=hidden \
		-std=c99 \
		-ffunction-sections \
		-fdata-sections \
		-I./libc/ -I./include/ -I./mrbgems/mruby-compiler/core/

$(OUTPUT): build/sys/mrblib/mrblib.c $(OBJ) $(MUSL) Makefile
	wasm-ld-8 \
		-o $(OUTPUT) \
		--strip-all \
		--export-dynamic \
		-error-limit=0 \
		--no-entry \
		--lto-O3 \
		-O3 \
		--gc-sections \
		libclang_rt.builtins-wasm32.a $(OBJ) $(MUSL)

%.o: %.c $(DEPS) Makefile
	clang-8 \
		-c \
		$(COMPILE_FLAGS) \
		-o $@ \
		$<

build/sys/mrblib/mrblib.c:
	MRUBY_CONFIG=../../build_config.rb ./minirake --jobs 4

clean:
	$(RM) -r $(OBJ) $(MUSL) $(OUTPUT) bin/mrbc build/

