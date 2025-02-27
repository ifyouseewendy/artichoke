//! [`Regexp#inspect`](https://ruby-doc.org/core-2.6.3/Regexp.html#method-i-inspect)

use crate::convert::{Convert, RustBackedValue};
use crate::extn::core::regexp::Regexp;
use crate::value::Value;
use crate::Artichoke;

#[derive(Debug, Clone, Copy, Hash, PartialEq, Eq)]
pub enum Error {
    Fatal,
}

pub fn method(interp: &Artichoke, value: &Value) -> Result<Value, Error> {
    let data = unsafe { Regexp::try_from_ruby(interp, value) }.map_err(|_| Error::Fatal)?;
    let borrow = data.borrow();
    let s = format!(
        "/{}/{}{}",
        borrow.literal_pattern.as_str().replace("/", r"\/"),
        borrow.literal_options.modifier_string(),
        borrow.encoding.string()
    );
    Ok(Value::convert(interp, s))
}
