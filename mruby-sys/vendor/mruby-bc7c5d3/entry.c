#include <stdlib.h>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/variable.h>

#define WASM_EXPORT __attribute__((visibility("default")))

/* SDK: string */
struct SliceUtf8 {
    char *data;
    unsigned length;
};

/* SDK: slice_of_int */
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

/* SDK: slice_of_string */
struct slice_of_string {
    struct SliceUtf8** data;
    uint32_t length;
};
mrb_value slice_of_string_each(mrb_state *mrb, mrb_value self) {
    mrb_value block;
    mrb_get_args(mrb, "&!", &block);

    struct slice_of_string *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    for(uint32_t i = 0; i < slice->length; i++) {
        struct SliceUtf8* s = slice->data[i];
        mrb_yield(mrb, block, mrb_str_new(mrb, s->data, s->length));
    }

    return self;
}
mrb_value slice_of_string_length(mrb_state *mrb, mrb_value self) {
    struct slice_of_string *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(slice->length);
}

/* MoneyInput */
struct MoneyInput {
    int cents;
    struct SliceUtf8* currency;
};
mrb_value money_input_cents(mrb_state *mrb, mrb_value self) {
    struct MoneyInput *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(ptr->cents);
}
mrb_value money_input_currency(mrb_state *mrb, mrb_value self) {
    struct MoneyInput *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->currency;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}

/* Product */
struct Product {
    uint64_t id;
    struct SliceUtf8* title;
    struct slice_of_string* tags;
};
mrb_value product_id(mrb_state *mrb, mrb_value self) {
    struct Product *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(ptr->id);
}
mrb_value product_title(mrb_state *mrb, mrb_value self) {
    struct Product *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->title;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}
mrb_value product_tags(mrb_state *mrb, mrb_value self) {
    struct Product *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    mrb_value nil = mrb_nil_value();
    struct RClass *mrb_sos_class = mrb_class_get(mrb, "SliceOfString");
    mrb_value mrb_tags_ptr = mrb_cptr_value(mrb, ptr->tags);
    mrb_value mrb_tags_obj = mrb_obj_new(mrb, mrb_sos_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_tags_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_tags_ptr);

    return mrb_tags_obj;
}

/* Variant */
struct Variant {
    uint64_t id;
    struct Product* product;
    struct MoneyInput* price;
    struct slice_of_string* skus;
};
mrb_value variant_id(mrb_state *mrb, mrb_value self) {
    struct Variant *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(ptr->id);
}
mrb_value variant_price(mrb_state *mrb, mrb_value self) {
    struct Variant *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_money_input_class = mrb_class_get(mrb, "MoneyInput");
    mrb_value nil = mrb_nil_value();

    mrb_value mrb_money_input_ptr = mrb_cptr_value(mrb, ptr->price);
    mrb_value mrb_money_input_obj = mrb_obj_new(mrb, mrb_money_input_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_money_input_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_money_input_ptr);

    return mrb_money_input_obj;
}
mrb_value variant_product(mrb_state *mrb, mrb_value self) {
    struct Variant *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_product_class = mrb_class_get(mrb, "Product");
    mrb_value nil = mrb_nil_value();

    mrb_value mrb_product_ptr = mrb_cptr_value(mrb, ptr->product);
    mrb_value mrb_product_obj = mrb_obj_new(mrb, mrb_product_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_product_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_product_ptr);

    return mrb_product_obj;
}
mrb_value variant_skus(mrb_state *mrb, mrb_value self) {
    struct Variant *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    mrb_value nil = mrb_nil_value();
    struct RClass *mrb_sos_class = mrb_class_get(mrb, "SliceOfString");
    mrb_value mrb_skus_ptr = mrb_cptr_value(mrb, ptr->skus);
    mrb_value mrb_skus_obj = mrb_obj_new(mrb, mrb_sos_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_skus_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_skus_ptr);

    return mrb_skus_obj;
}

/* Line Item */
struct LineItem {
    struct Variant* variant;
    int quantity;
    struct SliceUtf8* title;
    struct MoneyInput* price;
};
mrb_value line_item_quantity(mrb_state *mrb, mrb_value self) {
    struct LineItem *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(ptr->quantity);
}
mrb_value line_item_title(mrb_state *mrb, mrb_value self) {
    struct LineItem *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->title;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}
mrb_value line_item_price(mrb_state *mrb, mrb_value self) {
    struct LineItem *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_money_input_class = mrb_class_get(mrb, "MoneyInput");
    mrb_value nil = mrb_nil_value();

    mrb_value mrb_money_input_ptr = mrb_cptr_value(mrb, ptr->price);
    mrb_value mrb_money_input_obj = mrb_obj_new(mrb, mrb_money_input_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_money_input_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_money_input_ptr);

    return mrb_money_input_obj;
}
mrb_value line_item_variant(mrb_state *mrb, mrb_value self) {
    struct LineItem *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_variant_class = mrb_class_get(mrb, "Variant");
    mrb_value nil = mrb_nil_value();

    mrb_value mrb_variant_ptr = mrb_cptr_value(mrb, ptr->variant);
    mrb_value mrb_variant_obj = mrb_obj_new(mrb, mrb_variant_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_variant_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_variant_ptr);

    return mrb_variant_obj;
}

/* slice_of_line_item */
struct slice_of_line_item {
    struct LineItem** data;
    uint32_t length;
};
mrb_value slice_of_line_item_each(mrb_state *mrb, mrb_value self) {
    mrb_value block;
    mrb_get_args(mrb, "&!", &block);

    struct slice_of_line_item *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_li_class = mrb_class_get(mrb, "LineItem");
    mrb_value nil = mrb_nil_value();

    for(uint32_t i = 0; i < slice->length; i++) {
        struct LineItem* s = slice->data[i];

        mrb_value mrb_li_ptr = mrb_cptr_value(mrb, s);
        mrb_value mrb_li_obj = mrb_obj_new(mrb, mrb_li_class, 0, &nil);
        mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_li_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_li_ptr);

        mrb_yield(mrb, block, mrb_li_obj);
    }

    return self;
}
mrb_value slice_of_line_item_length(mrb_state *mrb, mrb_value self) {
    struct slice_of_line_item *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(slice->length);
}

/* DiscountCode */
struct DiscountCode {
    struct SliceUtf8* code;
    struct MoneyInput* amount;
    struct SliceUtf8* type;
};
mrb_value discount_code_code(mrb_state *mrb, mrb_value self) {
    struct DiscountCode *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->code;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}
mrb_value discount_code_amount(mrb_state *mrb, mrb_value self) {
    struct DiscountCode *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_money_input_class = mrb_class_get(mrb, "MoneyInput");
    mrb_value nil = mrb_nil_value();

    mrb_value mrb_money_input_ptr = mrb_cptr_value(mrb, ptr->amount);
    mrb_value mrb_money_input_obj = mrb_obj_new(mrb, mrb_money_input_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_money_input_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_money_input_ptr);

    return mrb_money_input_obj;
}
mrb_value discount_code_type(mrb_state *mrb, mrb_value self) {
    struct DiscountCode *ptr = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    struct SliceUtf8* s = ptr->type;

    mrb_value str = mrb_str_new(mrb, s->data, s->length);
    return str;
}

/* slice_of_discount_code */
struct slice_of_discount_code {
    struct DiscountCode** data;
    uint32_t length;
};
mrb_value slice_of_discount_code_each(mrb_state *mrb, mrb_value self) {
    mrb_value block;
    mrb_get_args(mrb, "&!", &block);

    struct slice_of_discount_code *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));

    struct RClass *mrb_dc_class = mrb_class_get(mrb, "DiscountCode");
    mrb_value nil = mrb_nil_value();

    for(uint32_t i = 0; i < slice->length; i++) {
        struct DiscountCode* s = slice->data[i];

        mrb_value mrb_dc_ptr = mrb_cptr_value(mrb, s);
        mrb_value mrb_dc_obj = mrb_obj_new(mrb, mrb_dc_class, 0, &nil);
        mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_dc_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_dc_ptr);

        mrb_yield(mrb, block, mrb_dc_obj);
    }

    return self;
}
mrb_value slice_of_discount_code_length(mrb_state *mrb, mrb_value self) {
    struct slice_of_discount_code *slice = mrb_cptr(mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@struct_ptr")));
    return mrb_fixnum_value(slice->length);
}

struct Cart {
    struct slice_of_line_item* line_items;
    struct slice_of_discount_code* discount_codes;
};
mrb_value cart_discount_codes(mrb_state *mrb, mrb_value self) {
    return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@_discount_codes"));
}
mrb_value cart_line_items(mrb_state *mrb, mrb_value self) {
    return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@_line_items"));
}

struct DiscountsExtensionRequest {
    struct Cart* checkout;
};
mrb_value request_checkout(mrb_state *mrb, mrb_value self) {
    return mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@_checkout"));
}

/* Output */
struct DiscountsExtensionResponse {
    /* struct slice_of_line_item* line_items; */
    /* int line_items; */
    struct LineItem* line_items;
};

WASM_EXPORT struct DiscountsExtensionResponse* run(struct DiscountsExtensionRequest* input) {
    mrb_state *mrb = mrb_open();
    if (!mrb) { abort(); }

    char* ruby_script = "\
                            class MoneyInput \n\
                            end \n\
                            class Product \n\
                            end \n\
                            class Variant \n\
                            end \n\
                            class LineItem \n\
                            end \n\
                            class DiscountCode \n\
                            end \n\
                            class SliceOfString \n\
                              include Enumerable \n\
                            end \n\
                            class SliceOfLineItem \n\
                              include Enumerable \n\
                            end \n\
                            class SliceOfDiscountCode \n\
                              include Enumerable \n\
                            end \n\
                            class Cart \n\
                            end \n\
                            class DiscountsExtensionRequest \n\
                            end \n\
                            class DiscountsExtensionResponse \n\
                              def initialize(line_items) \n\
                                @line_items = line_items \n\
                              end \n\
                            end \n\
                            \n\
                            ALL_TAGS = ['bulknaked', 'BULKSALTE', 'BULKGFALLS', 'BULKANML', 'BULKBAR', 'BULKBEARD'] \n\
                            class Pair \n\
                              attr_reader :first, :second \n\
                              def initialize(first, second) \n\
                                @first = first \n\
                                @second = second \n\
                              end \n\
                            end \n\
                            QUANTITY_DISCOUNT_MAPPING = [ \n\
                              Pair.new(10, 2.0), \n\
                              Pair.new(20, 5.0), \n\
                              Pair.new(30, 6.5), \n\
                              Pair.new(40, 8.0), \n\
                              Pair.new(50, 9.5) \n\
                            ] \n\
                            PRICE_LIST = {} \n\
                            ALL_TAGS.each { |tag| PRICE_LIST[tag] = QUANTITY_DISCOUNT_MAPPING } \n\
                             \n\
                            def find_discount_percentage(price_list, tag, total) \n\
                              discount_arr = price_list[tag] \n\
                              return 0 if discount_arr.empty? \n\
                             \n\
                              discount_percentage = 2.0 \n\
                              discount_arr.each do |pair| \n\
                                if pair.first <= total \n\
                                  discount_percentage = pair.second \n\
                                else \n\
                                  break \n\
                                end \n\
                              end \n\
                             \n\
                              return discount_percentage \n\
                            end \n\
                             \n\
                            class Adjustment \n\
                              def initialize(*) \n\
                              end \n\
                            end \n\
                             \n\
def run(req) \n\
  orig_line_items = req.checkout.line_items \n\
  # grouped_line_items = Hash.new { |h,k| h[k] = Array.new } won't work in mruby \n\
  # I'm simulating its behaviour here \n\
  tag_to_line_items = {} \n\
  grouped_line_items = [] \n\
  # array count is not working either, need to manually counting \n\
  line_items_count = 0 \n\
 \n\
  orig_line_items.each do |line_item| \n\
    tags = line_item.variant.product.tags \n\
    tags.each do |tag| \n\
      if PRICE_LIST.key?(tag) \n\
        if tag_to_line_items.key?(tag) \n\
          idx = tag_to_line_items[tag] \n\
          grouped_line_items[idx].push(line_item) \n\
        else \n\
          tag_to_line_items[tag] = line_items_count \n\
          grouped_line_items << [line_item] \n\
          line_items_count += 1 \n\
        end \n\
      end \n\
    end \n\
  end \n\
 \n\
  discount_rates = {} \n\
  tag_to_line_items.each do |(tag, idx)| \n\
    line_items = grouped_line_items[idx] \n\
    total = line_items.map(&:quantity).reduce(:+) \n\
    discount_percentage = find_discount_percentage(PRICE_LIST, tag, total) \n\
    discount_rates[tag] = discount_percentage \n\
  end \n\
 \n\
  adjustments = [] \n\
  count = 0 \n\
  orig_line_items.each do |line_item| \n\
    tags = line_item.variant.product.tags \n\
    price = line_item.price.cents \n\
    new_item_price = price \n\
    message = '' \n\
 \n\
    tags.each do |tag| \n\
      if discount_rates.key?(tag) \n\
        discount_percentage = discount_rates[tag] \n\
        if discount_percentage > 0 \n\
          message += 'Discount for ' + tag + ' tagged products' \n\
          new_item_price = new_item_price * (1 - discount_percentage / 100) \n\
          count += 1 \n\
          # break break is not working \n\
        end \n\
      end \n\
    end \n\
 \n\
    adjustments << Adjustment.new(line_item.variant.id, new_item_price, message) \n\
  end \n\
 \n\
  # Since there is no glue code for handling output yet, just return [] \n\
  DiscountsExtensionResponse.new(nil) \n\
end \n\
                        ";
    mrb_value script = mrb_load_string(mrb, ruby_script);

    /* == Class registriation */
    /* MoneyInput */
    struct RClass *mrb_money_input_class = mrb_class_get(mrb, "MoneyInput");
    mrb_define_method(mrb, mrb_money_input_class, "cents", &money_input_cents, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_money_input_class, "currency", &money_input_currency, MRB_ARGS_NONE());

    /* Product */
    struct RClass *mrb_product_class = mrb_class_get(mrb, "Product");
    mrb_define_method(mrb, mrb_product_class, "id", &product_id, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_product_class, "title", &product_title, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_product_class, "tags", &product_tags, MRB_ARGS_NONE());

    /* Variant */
    struct RClass *mrb_variant_class = mrb_class_get(mrb, "Variant");
    mrb_define_method(mrb, mrb_variant_class, "id", &variant_id, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_variant_class, "product", &variant_product, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_variant_class, "price", &variant_price, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_variant_class, "skus", &variant_skus, MRB_ARGS_NONE());

    /* LineItem */
    struct RClass *mrb_li_class = mrb_class_get(mrb, "LineItem");
    mrb_define_method(mrb, mrb_li_class, "quantity", &line_item_quantity, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_li_class, "title", &line_item_title, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_li_class, "price", &line_item_price, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_li_class, "variant", &line_item_variant, MRB_ARGS_NONE());

    /* DiscountCode */
    struct RClass *mrb_dc_class = mrb_class_get(mrb, "DiscountCode");
    mrb_define_method(mrb, mrb_dc_class, "code", &discount_code_code, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_dc_class, "amount", &discount_code_amount, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_dc_class, "type", &discount_code_type, MRB_ARGS_NONE());

    /* SliceOfString */
    struct RClass *mrb_sos_class = mrb_class_get(mrb, "SliceOfString");
    mrb_define_method(mrb, mrb_sos_class, "each", &slice_of_string_each, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_sos_class, "length", &slice_of_string_length, MRB_ARGS_NONE());

    /* SliceOfLineItem */
    struct RClass *mrb_soli_class = mrb_class_get(mrb, "SliceOfLineItem");
    mrb_define_method(mrb, mrb_soli_class, "each", &slice_of_line_item_each, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_soli_class, "length", &slice_of_line_item_length, MRB_ARGS_NONE());

    /* SliceOfDiscountCode */
    struct RClass *mrb_sodc_class = mrb_class_get(mrb, "SliceOfDiscountCode");
    mrb_define_method(mrb, mrb_sodc_class, "each", &slice_of_discount_code_each, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_sodc_class, "length", &slice_of_discount_code_length, MRB_ARGS_NONE());

    /* Cart */
    struct RClass *mrb_cart_class = mrb_class_get(mrb, "Cart");
    mrb_define_method(mrb, mrb_cart_class, "discount_codes", &cart_discount_codes, MRB_ARGS_NONE());
    mrb_define_method(mrb, mrb_cart_class, "line_items", &cart_line_items, MRB_ARGS_NONE());

    /* DiscountsExtensionRequest */
    struct RClass *mrb_request_class = mrb_class_get(mrb, "DiscountsExtensionRequest");
    mrb_define_method(mrb, mrb_request_class, "checkout", &request_checkout, MRB_ARGS_NONE());
    /* == Class registriation end */

    /* == Initialize input */
    mrb_value nil = mrb_nil_value();

    /* discount_codes */
    mrb_value mrb_sodc_ptr = mrb_cptr_value(mrb, input->checkout->discount_codes);
    mrb_value mrb_sodc_obj = mrb_obj_new(mrb, mrb_sodc_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_sodc_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_sodc_ptr);

    /* line_items */
    mrb_value mrb_soli_ptr = mrb_cptr_value(mrb, input->checkout->line_items);
    mrb_value mrb_soli_obj = mrb_obj_new(mrb, mrb_soli_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_soli_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_soli_ptr);

    /* cart */
    mrb_value mrb_cart_ptr = mrb_cptr_value(mrb, input->checkout);
    mrb_value mrb_cart_obj = mrb_obj_new(mrb, mrb_cart_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_cart_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_cart_ptr);

    /* request */
    mrb_value mrb_request_ptr = mrb_cptr_value(mrb, input);
    mrb_value mrb_request_obj = mrb_obj_new(mrb, mrb_request_class, 0, &nil);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_request_obj), mrb_intern_lit(mrb, "@struct_ptr"), mrb_request_ptr);

    /* bind togehter */
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_cart_obj), mrb_intern_lit(mrb, "@_line_items"), mrb_soli_obj);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_cart_obj), mrb_intern_lit(mrb, "@_discount_codes"), mrb_sodc_obj);
    mrb_obj_iv_set_force(mrb, mrb_ptr(mrb_request_obj), mrb_intern_lit(mrb, "@_checkout"), mrb_cart_obj);
    /* == Initialize input end */

    /* Run */
    mrb_value rv = mrb_funcall_argv(mrb, script, mrb_intern_lit(mrb, "run"), 1, &mrb_request_obj);

    /* Handle output */
    mrb_value ret = mrb_iv_get(mrb, rv, mrb_intern_lit(mrb, "@line_items"));

    struct DiscountsExtensionResponse *response = malloc(sizeof(struct DiscountsExtensionResponse));

    /* struct slice_of_line_item* result = malloc(sizeof(struct slice_of_line_item)); */
    /* result->data = (struct LineItem**)RARRAY_PTR(ret); */
    /* result->length = RARRAY_LEN(ret); */
    /* response->line_items = result; */

    /* response->line_items = mrb_fixnum(ret); */

    response->line_items = NULL;

    mrb_close(mrb);
    return response;
}

WASM_EXPORT void* shopify_runtime_allocate(size_t s) {
    return malloc(s);
}
