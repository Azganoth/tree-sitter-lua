# tree-sitter-lua

This crate provides Lua language support for the [tree-sitter] parsing library.

To use this crate, add it to the `[dependencies]` section of your `Cargo.toml`
file.

``` toml
[dependencies]
tree-sitter = "0.20"
tree-sitter-lua = { git = "https://github.com/Azganoth/tree-sitter-lua" }
```

> Note that you will probably also need to depend on the [tree-sitter crate] to
use the parsed result in any useful way.

Typically, you will use the language function to add this language to a
tree-sitter [Parser][tree-sitter parser], and then use the parser to parse
some code:

```rust
let code = r#"
function double(x)
    return x * 2
end
"#;

let mut parser = tree_sitter::Parser::new();
parser
    .set_language(tree_sitter_lua::language())
    .expect("Error loading Lua grammar");

let tree = parser.parse(code, None).expect("Error parsing Lua code");
println!("{tree:?}");
```

If you have any questions, please use the [tree-sitter discussions] page.

[tree-sitter]: https://tree-sitter.github.io/
[tree-sitter crate]: https://crates.io/crates/tree-sitter
[tree-sitter parser]: https://docs.rs/tree-sitter/*/tree_sitter/struct.Parser.html
[tree-sitter discussions]: https://github.com/tree-sitter/tree-sitter/discussions
