#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>

#define WASM_EXPORT __attribute__((visibility("default")))

/*  */
/* struct sliceint { */
/*     int *data; */
/*     unsigned length; */
/* } */

struct SliceUtf8 {
    char *data;
    unsigned length;
};

WASM_EXPORT struct SliceUtf8* run(struct SliceUtf8* s) {
	mrb_state *mrb = mrb_open();
	if (!mrb) { abort(); }

        // Load function
	mrb_value func = mrb_load_string(mrb, "def run(str); \"hello #{str}\"; end");
        mrb_sym sym = mrb_intern_lit(mrb, "run"); // Symbol for method.

        // Prepare argument
        mrb_value str = mrb_str_new_static(mrb, s->data, s->length);

        // Call function and prepare return
        mrb_value ret_str = mrb_funcall_argv(mrb, func, sym, 1, &str);

        struct SliceUtf8 slice = {
            .data = RSTRING_PTR(ret_str),
            .length = RSTRING_LEN(ret_str)
        };

	mrb_close(mrb);

        return &slice;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
	return malloc(s);
}
