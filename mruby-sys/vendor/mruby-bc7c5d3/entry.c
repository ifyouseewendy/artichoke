#include <stdlib.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/variable.h>

#define WASM_EXPORT __attribute__((visibility("default")))

/* struct Input { */
/*     int price; */
/* }; */
/*  */
/* mrb_value input_price(mrb_state *mrb, mrb_value self) { */
/*     struct Input *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr"))); */
/*     return mrb_fixnum_value(ptr->price); */
/* } */

struct Output {
    int price;
};
mrb_value output_price(mrb_state *mrb, mrb_value self) {
    struct Output *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@price")));
    return mrb_fixnum_value(ptr->price);
}

/* mrb_value output_new(mrb_state *mrb, mrb_value self) { */
/*     mrb_int i = 42; */
/*     mrb_iv_set(mrb, self, mrb_intern_lit(mrb, "@price"), mrb_fixnum_value(i)); */
/*  */
/*     return self; */
/* } */

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

WASM_EXPORT int run(int price) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    mrb_value script = mrb_load_string(mrb, "class Output; def price; @price; end; def initialize; @price = 42; end; end; def run(num); Output.new; end");
    /* struct RClass *mrb_output = mrb_class_get(mrb, "Output"); */
    /* mrb_define_method(mrb, mrb_output, "price", &output_price, MRB_ARGS_NONE()); */

    mrb_int i = 42;
    mrb_value p = mrb_fixnum_value(i);
    mrb_value rv = mrb_funcall_argv(mrb, script, mrb_intern_lit(mrb, "run"), 1, &p);

    return mrb_fixnum(mrb_iv_get(mrb, rv, mrb_intern_lit(mrb, "@price")));
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
