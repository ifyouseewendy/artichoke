#include <mruby.h>
#include <mruby/compile.h>

#define WASM_EXPORT __attribute__((visibility("default")))

WASM_EXPORT int run(int x) {
	mrb_state *mrb = mrb_open();
	if (!mrb) { abort(); }
	mrb_value rv = mrb_load_string(mrb, "10 * 10");
	int r = mrb_fixnum(rv);
	mrb_close(mrb);
	return r;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
	return malloc(s);
}
