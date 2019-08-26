#include <stdlib.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>

#define WASM_EXPORT __attribute__((visibility("default")))

struct Input {
    int price;
};

mrb_value input_price(mrb_state *mrb, mrb_value self) {
    struct Input *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(ptr->price);
}

/* struct slice_of_int { */
/*     int32_t *data; */
/*     uint32_t length; */
/* }; */
/*  */
/* mrb_value slice_of_int_each(mrb_state *mrb, mrb_value self) { */
/*     mrb_value block; */
/*     mrb_get_args(mrb, "&!", &block); */
/*  */
/*     struct slice_of_int *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr"))); */
/*  */
/*     for(uint32_t i = 0; i < slice->length; i++) { */
/*         mrb_yield(mrb, block, mrb_fixnum_value(slice->data[i])); */
/*     } */
/*  */
/*     return self; */
/* } */
/*  */
/* mrb_value slice_of_int_length(mrb_state *mrb, mrb_value self) { */
/*     struct slice_of_int *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr"))); */
/*     return mrb_fixnum_value(slice->length); */
/* } */

WASM_EXPORT int run(struct Input *input) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    mrb_value script = mrb_load_string(mrb, "class Input; def initialize(struct_ptr); @struct_ptr = struct_ptr; end; end; def run(input); input.price; end");
    struct RClass *mrb_input = mrb_class_get(mrb, "Input");
    mrb_define_method(mrb, mrb_input, "price", &input_price, MRB_ARGS_NONE());
    mrb_value v = mrb_cptr_value(mrb, input);
    mrb_value arg = input ? mrb_obj_new(mrb, mrb_input, 1, &v) : mrb_nil_value();

    mrb_value rv = mrb_funcall_argv(mrb, script, mrb_intern_lit(mrb, "run"), 1, &arg);

    mrb_close(mrb);
    return mrb_fixnum(rv);
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
