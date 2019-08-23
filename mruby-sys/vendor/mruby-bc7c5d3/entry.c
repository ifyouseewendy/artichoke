#include <stdlib.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>

#define WASM_EXPORT __attribute__((visibility("default")))

struct slice_of_int {
    int32_t *data;
    uint32_t length;
};

mrb_value slice_of_int_each(mrb_state *mrb, mrb_value self) {
    mrb_value block;
    mrb_get_args(mrb, "&!", &block);

    struct slice_of_int *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    for(uint32_t i = 0; i < slice->length; i++) {
        mrb_yield(mrb, block, mrb_fixnum_value(slice->data[i]));
    }

    return self;
}

mrb_value slice_of_int_length(mrb_state *mrb, mrb_value self) {
    struct slice_of_int *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(slice->length);
}

WASM_EXPORT int run(struct slice_of_int *slice) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    mrb_value top = mrb_load_string(mrb, "class SliceOfInt; include Enumerable; def initialize(struct_ptr); @struct_ptr = struct_ptr; end; end; def run(slice); slice.reduce(:+); end");
    struct RClass *mrb_slice_of_int = mrb_class_get(mrb, "SliceOfInt");
    mrb_define_method(mrb, mrb_slice_of_int, "each", &slice_of_int_each, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_slice_of_int, "length", &slice_of_int_length, MRB_ARGS_NONE());

    mrb_value arg = slice ? mrb_obj_new(mrb, mrb_slice_of_int, 1, (mrb_value[]){ mrb_cptr_value(mrb, slice) }) : mrb_nil_value();
    mrb_value rv = mrb_funcall_argv(mrb, top, mrb_intern_lit(mrb, "run"), 1, &arg);
    mrb_close(mrb);
    return mrb_fixnum(rv);
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
