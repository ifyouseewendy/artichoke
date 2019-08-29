This is a fork of [artichoke/artichoke](https://github.com/artichoke/artichoke), which we compile mruby
to wasm. All changes are in [mruby-sys/vendor/mruby-bc7c5d3/](./mruby-sys/vendor/mruby-bc7c5d3/). Please
cd into this directory first.

As we know, with the help of LLVM, we are able to compile a C program into WASM. So, what if the C
program does is to init a Ruby interpreter and run a Ruby script on it? Yeah, that's what we are doing
in this project. Practically, we are compiling a C program here. It includes a mruby interpreter, a Ruby script
and some glue code to transforming C values back and forth from the Ruby script execution.

`entry.c` is the main entry file. `test.schema` is the schema file, which is a copy of vanity pricing schema. To
make it running,

1. Init Runtime Engine in release mode, by `env MEMCACHED_SERVERS=runtime-engine.railgun cargo run --release`
2. Compile, deploy and run by `./run`
