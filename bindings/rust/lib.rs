use tree_sitter::Language;

extern "C" {
    fn tree_sitter_lua() -> Language;
}

pub fn language() -> Language {
    unsafe { tree_sitter_lua() }
}

pub const NODE_TYPES: &'static str = include_str!("../../src/node-types.json");

// pub const HIGHLIGHTS_QUERY: &'static str = include_str!("../../queries/highlights.scm");
// pub const INJECTIONS_QUERY: &'static str = include_str!("../../queries/injections.scm");
// pub const LOCALS_QUERY: &'static str = include_str!("../../queries/locals.scm");
// pub const TAGS_QUERY: &'static str = include_str!("../../queries/tags.scm");

#[cfg(test)]
mod tests {
    #[test]
    fn test_can_load_grammar() {
        let mut parser = tree_sitter::Parser::new();
        parser
            .set_language(super::language())
            .expect("Error loading Lua language");
    }
}
