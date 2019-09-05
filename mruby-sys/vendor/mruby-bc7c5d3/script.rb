DATA.each_line do |line|
  puts "#{line.chop} \\n\\"
end

__END__
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
