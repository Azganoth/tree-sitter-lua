const PREC = {
  OR: 1, // "or"
  AND: 2, // "and"
  COMPARATIVE: 3, // "==" "~=" "<" ">" "<=" ">="
  BIT_OR: 4, // "|"
  BIT_NOT: 5, // "~"
  BIT_AND: 6, // "&"
  BIT_SHIFT: 7, // "<<" ">>"
  CONCATENATION: 8, // ".."
  ADDITIVE: 9, // "+" "-"
  MULTIPLICATIVE: 10, // "*" "/" "//" "%"
  UNARY: 11, // "not" "#" "-" "~"
  POWER: 12, // "^"
};

const WHITESPACE = /\s/;
const IDENTIFIER = /[a-zA-Z_][0-9a-zA-Z_]*/;
const DECIMAL_DIGIT = /[0-9]/;
const HEXADECIMAL_DIGIT = /[0-9a-fA-F]/;
const SHEBANG = /#!.*/;

const _numeral = (digit) =>
  choice(
    repeat1(digit),
    seq(repeat1(digit), ".", repeat(digit)),
    seq(repeat(digit), ".", repeat1(digit)),
  );

const _exponent_part = (...delimiters) =>
  seq(
    choice(...delimiters),
    optional(choice("+", "-")),
    repeat1(DECIMAL_DIGIT),
  );

const _list = (rule, separator) => seq(rule, repeat(seq(separator, rule)));

module.exports = grammar({
  name: "lua",

  rules: {
    chunk: ($) => seq(optional($.shebang), optional($._block)),

    shebang: ($) => SHEBANG,

    block: ($) => $._block,
    _block: ($) =>
      choice(
        $.return_statement,
        seq(repeat1($.statement), optional($.return_statement)),
      ),

    // statements
    return_statement: ($) =>
      seq("return", optional($.expression_list), optional($.empty_statement)),

    statement: ($) =>
      choice(
        $.empty_statement,
        $.variable_assignment,
        $.scoped_variable_declaration,
        // conflict: $.statement and $.prefix_expression
        prec(1, $.function_call),
        $.label_statement,
        $.goto_statement,
        $.break_statement,
        $.do_statement,
        $.while_statement,
        $.repeat_statement,
        $.if_statement,
        $.for_numeric_statement,
        $.for_generic_statement,
        $.function_definition_statement,
        $.scoped_function_definition_statement,
      ),

    scoped_function_definition_statement: ($) =>
      seq("local", "function", field("name", $.identifier), $._function_body),

    function_definition_statement: ($) =>
      seq("function", field("name", $.function_identifier), $._function_body),
    function_identifier: ($) =>
      seq(
        _list($.identifier, "."),
        optional(seq(":", field("method", $.identifier))),
      ),

    for_generic_statement: ($) =>
      seq(
        "for",
        field("left", alias($._name_list, $.variable_list)),
        "in",
        field("right", alias($._value_list, $.expression_list)),
        "do",
        optional(field("body", $.block)),
        "end",
      ),
    _name_list: ($) => _list(field("name", $.identifier), ","),
    _value_list: ($) => _list(field("value", $.expression), ","),

    for_numeric_statement: ($) =>
      seq(
        "for",
        field("name", $.identifier),
        "=",
        field("start", $.expression),
        ",",
        field("end", $.expression),
        optional(seq(",", field("step", $.expression))),
        "do",
        optional(field("body", $.block)),
        "end",
      ),

    if_statement: ($) =>
      seq(
        "if",
        field("condition", $.expression),
        "then",
        optional(field("consequence", $.block)),
        repeat(field("alternative", $.elseif_clause)),
        optional(field("alternative", $.else_clause)),
        "end",
      ),
    elseif_clause: ($) =>
      seq(
        "elseif",
        field("condition", $.expression),
        "then",
        optional(field("consequence", $.block)),
      ),
    else_clause: ($) => seq("else", optional(field("body", $.block))),

    repeat_statement: ($) =>
      seq(
        "repeat",
        optional(field("body", $.block)),
        "until",
        field("condition", $.expression),
      ),

    while_statement: ($) =>
      seq(
        "while",
        field("condition", $.expression),
        "do",
        optional(field("body", $.block)),
        "end",
      ),

    do_statement: ($) => seq("do", optional(field("body", $.block)), "end"),

    break_statement: () => "break",

    goto_statement: ($) => seq("goto", field("name", $.identifier)),
    label_statement: ($) => seq("::", field("name", $.identifier), "::"),

    scoped_variable_declaration: ($) =>
      seq(
        "local",
        alias($._scoped_variable_list, $.variable_list),
        optional(seq("=", alias($._value_list, $.expression_list))),
      ),
    _scoped_variable_list: ($) =>
      _list(alias($._scoped_variable, $.variable), ","),
    _scoped_variable: ($) =>
      seq(field("name", $.identifier), optional($.attribute)),
    attribute: ($) => seq("<", field("name", $.identifier), ">"),

    variable_assignment: ($) =>
      seq($.variable_list, "=", alias($._value_list, $.expression_list)),
    variable_list: ($) => _list($.variable, ","),

    empty_statement: () => ";",

    // expressions
    expression: ($) =>
      choice(
        $.nil,
        $.false,
        $.true,
        $.number,
        $.string,
        $.vararg_expression,
        $.function_definition,
        // conflict: $.expression and $.function_call
        prec(1, $.prefix_expression),
        $.table,
        $.unary_expression,
        $.binary_expression,
      ),

    binary_expression: ($) =>
      choice(
        ...[
          ["or", PREC.OR],
          ["and", PREC.AND],
          ["==", PREC.COMPARATIVE],
          ["~=", PREC.COMPARATIVE],
          ["<", PREC.COMPARATIVE],
          [">", PREC.COMPARATIVE],
          ["<=", PREC.COMPARATIVE],
          [">=", PREC.COMPARATIVE],
          ["|", PREC.BIT_OR],
          ["~", PREC.BIT_NOT],
          ["&", PREC.BIT_AND],
          ["<<", PREC.BIT_SHIFT],
          [">>", PREC.BIT_SHIFT],
          ["+", PREC.ADDITIVE],
          ["-", PREC.ADDITIVE],
          ["*", PREC.MULTIPLICATIVE],
          ["/", PREC.MULTIPLICATIVE],
          ["//", PREC.MULTIPLICATIVE],
          ["%", PREC.MULTIPLICATIVE],
        ].map(([operator, priority]) =>
          prec.left(
            priority,
            seq(
              field("left", $.expression),
              field("operator", operator),
              field("right", $.expression),
            ),
          ),
        ),
        ...[
          ["..", PREC.CONCATENATION],
          ["^", PREC.POWER],
        ].map(([operator, priority]) =>
          prec.right(
            priority,
            seq(
              field("left", $.expression),
              field("operator", operator),
              field("right", $.expression),
            ),
          ),
        ),
      ),

    unary_expression: ($) =>
      choice(
        ...["not", "#", "-", "~"].map((operator) =>
          prec.left(
            PREC.UNARY,
            seq(field("operator", operator), field("argument", $.expression)),
          ),
        ),
      ),

    table: ($) => seq("{", optional($.field_list), "}"),
    field_list: ($) =>
      seq(_list($.field, $.field_separator), optional($.field_separator)),
    field: ($) =>
      choice(
        seq(
          "[",
          field("key", $.expression),
          "]",
          "=",
          field("value", $.expression),
        ),
        seq(field("key", $.identifier), "=", field("value", $.expression)),
        field("value", $.expression),
      ),
    field_separator: () => choice(",", ";"),

    prefix_expression: ($) =>
      choice($.variable, $.function_call, $.parenthesized_expression),
    parenthesized_expression: ($) => seq("(", $.expression, ")"),
    function_call: ($) =>
      seq(
        $.prefix_expression,
        optional(seq(":", field("method", $.identifier))),
        field("arguments", $.argument_list),
      ),
    argument_list: ($) =>
      choice(seq("(", optional($.expression_list), ")"), $.table, $.string),
    expression_list: ($) => _list($.expression, ","),
    variable: ($) =>
      choice(
        field("name", $.identifier),
        seq(
          field("table", $.prefix_expression),
          "[",
          field("field", $.expression),
          "]",
        ),
        seq(
          field("table", $.prefix_expression),
          ".",
          field("field", $.identifier),
        ),
      ),

    function_definition: ($) => seq("function", $._function_body),
    _function_body: ($) =>
      seq(
        "(",
        optional(field("parameters", $.parameter_list)),
        ")",
        optional(field("body", $.block)),
        "end",
      ),
    parameter_list: ($) =>
      choice(
        seq(
          _list(field("name", $.identifier), ","),
          optional(seq(",", $.vararg_expression)),
        ),
        $.vararg_expression,
      ),

    vararg_expression: () => "...",

    string: ($) =>
      seq($._string_start, optional($._string_content), $._string_end),

    number: ($) =>
      token(
        seq(
          optional("-"),
          choice(
            seq(_numeral(DECIMAL_DIGIT), optional(_exponent_part("e", "E"))),
            seq(
              choice("0x", "0X"),
              _numeral(HEXADECIMAL_DIGIT),
              optional(_exponent_part("p", "P")),
            ),
          ),
        ),
      ),

    true: () => "true",

    false: () => "false",

    nil: () => "nil",

    identifier: () => IDENTIFIER,

    // extras
    comment: ($) =>
      seq($._comment_start, optional($._comment_content), $._comment_end),
  },

  extras: ($) => [WHITESPACE, $.comment],

  externals: ($) => [
    $._comment_start,
    $._comment_content,
    $._comment_end,
    $._string_start,
    $._string_content,
    $._string_end,
  ],

  inline: ($) => [$.field_separator],

  supertypes: ($) => [$.prefix_expression, $.expression, $.statement],

  word: ($) => $.identifier,
});
