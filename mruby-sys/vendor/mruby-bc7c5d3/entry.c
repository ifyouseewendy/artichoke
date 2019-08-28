#include <stdlib.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#define WASM_EXPORT __attribute__((visibility("default")))

/* SDK */
struct SliceUtf8 {
    char *data;
    unsigned length;
};

/* Input */
struct MoneyInput {
    int subunits;
};
mrb_value mi_subunits(mrb_state *mrb, mrb_value self) {
    /* mrb_value struct_ptr = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")); */
    /*  */
    /* if (mrb_nil_p(struct_ptr)) { */
    /*     return mrb_fixnum_value(0); */
    /* } */

    /* return mrb_fixnum_value(ptr->subunits); */
    return mrb_fixnum_value(42);
}

struct MultiCurrencyRequest {
    struct MoneyInput* money;
    struct SliceUtf8* presentment_currency;
    struct SliceUtf8* shop_currency;
};
mrb_value mcr_money(mrb_state *mrb, mrb_value self) {
    mrb_value o = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@_money"));

    return o;
}
mrb_value mcr_presentment_currency(mrb_state *mrb, mrb_value self) {
    struct MultiCurrencyRequest *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->presentment_currency;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}
mrb_value mcr_shop_currency(mrb_state *mrb, mrb_value self) {
    struct MultiCurrencyRequest *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->shop_currency;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}

/* Output */
struct Money {
    int subunits;
    struct SliceUtf8* iso_currency;
};

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

WASM_EXPORT struct Money* run(struct MultiCurrencyRequest* mcr) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    char* ruby_script = "\
                            class MultiCurrencyRequest \n\
                            end \n\
                            class MoneyInput \n\
                            end \n\
                            class Money \n\
                              def initialize(subunits) \n\
                                @subunits = subunits \n\
                                @iso_currency = 'hello world' \n\
                              end \n\
                            end \n\
                            def run(input) \n\
                              if input.presentment_currency == 'CAD' && input.shop_currency == 'CAD' \n\
                                Money.new(input.money.subunits) \n\
                              else \n\
                                Money.new(3) \n\
                              end \n\
                            end \n\
                        ";
    mrb_value script = mrb_load_string(mrb, ruby_script);

    /* define class */
    struct RClass *mrb_mcr_class = mrb_class_get(mrb, "MultiCurrencyRequest");
    mrb_define_method(mrb, mrb_mcr_class, "money", &mcr_money, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_mcr_class, "presentment_currency", &mcr_presentment_currency, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_mcr_class, "shop_currency", &mcr_shop_currency, MRB_ARGS_NONE());

    struct RClass *mrb_mi_class = mrb_class_get(mrb, "MoneyInput");
    mrb_define_method(mrb, mrb_mi_class, "subunits", &mi_subunits, MRB_ARGS_NONE());

    /* Initialize input */
    /* init MultiCurrencyRequest object */
    mrb_value mrb_mcr_ptr = mrb_cptr_value(mrb, mcr);
    mrb_value nil = mrb_nil_value();
    mrb_value mrb_mcr_obj = mcr ? mrb_obj_new(mrb, mrb_mcr_class, 0, &nil) : mrb_nil_value();
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_mcr_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_mcr_ptr);

    /* init MoneyInput object */
    mrb_value mrb_mi_ptr = mrb_cptr_value(mrb, mcr->money);
    mrb_value mrb_mi_obj = mrb_obj_new(mrb, mrb_mi_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_mi_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_mi_ptr);

    /* save mi object as instance variable @_money of mcr object */
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_mcr_obj), mrb_intern_lit(mrb, "@_money"), mrb_mi_obj);

    /* run */
    mrb_value rv = mrb_funcall_argv(mrb, script, mrb_intern_lit(mrb, "run"), 1, &mrb_mcr_obj);

    /* handle output */
    mrb_value subunits = mrb_iv_get(mrb, rv, mrb_intern_lit(mrb, "@subunits"));
    mrb_value iso_currency = mrb_iv_get(mrb, rv, mrb_intern_lit(mrb, "@iso_currency"));

    struct Money *ret = malloc(sizeof(struct Money));

    ret->subunits = mrb_int(mrb, subunits);

    struct SliceUtf8 slice = {
        .data = (char*)RSTRING_PTR(iso_currency),
        .length = RSTRING_LEN(iso_currency)
    };
    ret->iso_currency = &slice;

    mrb_close(mrb);

    return ret;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
