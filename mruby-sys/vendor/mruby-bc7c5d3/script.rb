DATA.each_line do |line|
  puts "#{line.strip} \\n\\"
end

__END__
class MultiCurrencyRequest
end
class MoneyInput
end
class Money
  def initialize(subunits, iso_currency)
    @subunits = subunits
    @iso_currency = iso_currency
  end
end

def run(req)
  if req.money.subunits % 10 >= 5
    Money.new(req.money.subunits + 10 - req.money.subunits % 10, req.money.iso_currency)
  else
    Money.new(req.money.subunits - req.money.subunits % 10, req.money.iso_currency)
  end
end
