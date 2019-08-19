#include <mruby.h>
#include <mruby/compile.h>

#define WASM_EXPORT __attribute__((visibility("default")))

/* struct sliceutf8 { */
/*     char *str; */
/*     unsigned length; */
/* } */
/*  */
/* struct sliceint { */
/*     int *str; */
/*     unsigned length; */
/* } */

WASM_EXPORT int run(int x) {
	mrb_state *mrb = mrb_open();
	if (!mrb) { abort(); }

	mrb_value obj = mrb_load_string(mrb, "def run(x); x * 10; end");
        mrb_sym m_sym = mrb_intern_lit(mrb, "run"); // Symbol for method.
        mrb_int i = x;
        mrb_value p = mrb_fixnum_value(i);
        mrb_value ret = mrb_funcall_argv(mrb, obj, m_sym, 1, &p); // Calling ruby function from test.rb.
	int r = mrb_fixnum(ret);

	mrb_close(mrb);
	return r;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
	return malloc(s);
}
