<h1 align=center>simple-alloc</h1>
<p align=center><i>Small and fast and simple implementation of `malloc`/`free` for WASM projects</i></p>

### About

This project provides a replacement for `malloc`/`free` that are used by default when [WebAssembly](https://webassembly.org/) binary is built with [Emsctipten](https://emscripten.org/).

Why? According to [twiggy](https://github.com/rustwasm/twiggy) report `dlmalloc` is 6013 bytes long and `dlfree` is 1709. 7722 bytes in total. Not a lot... until it is 30%+ of the total binary size.

This implementation is much simpler and shorter: `malloc` is 388 bytes, `free` is 56 bytes, 444 bytes in total — way better!

And in the [project](https://github.com/eustas/2im) I've created this library for it works even faster than the default one.

There are just 50 lines of code. What is the purpose of simplicity? So that you can read and understand it and modify, to make it a perfect fit for your project.

> **Nota bene**: this is neither the simplest (and smallest), nor the fastest implementation; it is just practical. 

### How to use

Just add `simple-alloc.cc` as compilation unit. Thats it. You can even copy-paste file contents into one of your source files, e.g. "main".

### How it works

TODO: explain every line of the source code
