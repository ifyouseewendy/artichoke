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

struct Output {
    int price;
};
/* mrb_value output_price(mrb_state *mrb, mrb_value self) { */
/*     return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@price")); */
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

WASM_EXPORT struct Output* run(struct Input* input) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    char* ruby_script = "\
                            class Input \n\
                              def initialize(struct_ptr) \n\
                                @struct_ptr = struct_ptr \n\
                              end \n\
                            end \n\
                            class Output \n\
                              def initialize(price) \n\
                                @price = price \n\
                              end \n\
                            end \n\
                            def run(input) \n\
                              Output.new(input.price) \n\
                            end \n\
                        ";
    mrb_value script = mrb_load_string(mrb, ruby_script);

    struct RClass *mrb_input = mrb_class_get(mrb, "Input");
    mrb_define_method(mrb, mrb_input, "price", &input_price, MRB_ARGS_NONE());

    mrb_value v = mrb_cptr_value(mrb, input);
    mrb_value arg = input ? mrb_obj_new(mrb, mrb_input, 1, &v) : mrb_nil_value();
    mrb_value rv = mrb_funcall_argv(mrb, script, mrb_intern_lit(mrb, "run"), 1, &arg);

    mrb_value price = mrb_iv_get(mrb, rv, mrb_intern_lit(mrb, "@price"));

    struct Output *ret = malloc(sizeof(struct Output));
    ret->price = mrb_int(mrb, price);

    mrb_close(mrb);

    return ret;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
