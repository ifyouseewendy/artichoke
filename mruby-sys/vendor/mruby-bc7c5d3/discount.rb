# The TS version is here
# https://github.com/Shopify/runtime-assembly-scripts/blob/ffc92afed871d7c9b82f6fa7999a2efc5fc35a91/src/discounts-extension/new-vape.ts
ALL_TAGS = ['bulknaked', 'BULKSALTE', 'BULKGFALLS', 'BULKANML', 'BULKBAR', 'BULKBEARD']
class Pair
  attr_reader :first, :second
  def initialize(first, second)
    @first = first
    @second = second
  end
end
QUANTITY_DISCOUNT_MAPPING = [
  Pair.new(10, 2.0),
  Pair.new(20, 5.0),
  Pair.new(30, 6.5),
  Pair.new(40, 8.0),
  Pair.new(50, 9.5)
]
PRICE_LIST = {}
ALL_TAGS.each { |tag| PRICE_LIST[tag] = QUANTITY_DISCOUNT_MAPPING }

def find_discount_percentage(price_list, tag, total)
  discount_arr = price_list[tag]
  return 0 if discount_arr.empty?

  discount_percentage = 2.0
  discount_arr.each do |pair|
    if pair.first <= total
      discount_percentage = pair.second
    else
      break
    end
  end

  return discount_percentage
end

class Adjustment
  def initialize(*)
  end
end

def run(req)
  orig_line_items = req.checkout.line_items

  # grouped_line_items = Hash.new { |h,k| h[k] = Array.new } won't work in mruby
  # I'm simulating its behaviour here
  tag_to_line_items = {}
  grouped_line_items = []
  # array count is not working either, need to manually counting
  line_items_count = 0

  orig_line_items.each do |line_item|
    tags = line_item.variant.product.tags
    tags.each do |tag|
      if PRICE_LIST.key?(tag)
        if tag_to_line_items.key?(tag)
          idx = tag_to_line_items[tag]
          grouped_line_items[idx].push(line_item)
        else
          tag_to_line_items[tag] = line_items_count
          grouped_line_items << [line_item]
          line_items_count += 1
        end
      end
    end
  end

  discount_rates = {}
  tag_to_line_items.each do |(tag, idx)|
    line_items = grouped_line_items[idx]
    total = line_items.map(&:quantity).reduce(:+)
    discount_percentage = find_discount_percentage(PRICE_LIST, tag, total)
    discount_rates[tag] = discount_percentage
  end

  adjustments = []
  count = 0
  orig_line_items.each do |line_item|
    tags = line_item.variant.product.tags
    price = line_item.price.to_i
    new_item_price = price
    message = ''

    tags.each do |tag|
      if discount_rates.key?(tag)
        discount_percentage = discount_rates[tag]
        if discount_percentage > 0
          message += 'Discount for ' + tag + ' tagged products'
          new_item_price = new_item_price * (1 - discount_percentage / 100)
          count += 1
          # break break is not working
        end
      end
    end

    adjustments << Adjustment.new(line_item.variant.id, new_item_price, message)
  end

  # Since there is no glue code for handling output yet, just return []
  DiscountsExtensionResponse.new([])
end

DiscountsExtensionResponse = Struct.new(:a)

# Test code
Request = Struct.new(:checkout)
Checkout = Struct.new(:line_items)
LineItem = Struct.new(:variant, :quantity, :price)
Variant = Struct.new(:id, :product)
Product = Struct.new(:tags)

product_1 = Product.new(["tag1", "tag2", "bulknaked"])
product_2 = Product.new(["tag3", "tag4"])
variant_1 = Variant.new(1, product_1)
variant_2 = Variant.new(2, product_2)

li1 = LineItem.new(variant_1, 1, 1.0)
li2 = LineItem.new(variant_2, 2, 2.0)
line_items = [li1, li2]
checkout = Checkout.new(line_items)
req = Request.new(checkout)

p run(req)
puts "hello"
