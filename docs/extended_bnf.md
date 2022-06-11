# Lua syntax

Complete Lua syntax in [extended BNF][extended bnf], adapted from it’s
[manual][lua manual].

```ebnf
chunk = [block];

block = return_statement | statement {statement} [return_statement];

(* statements *)
return_statement = "return" [expression_list] [empty_statement];

statement = do_statement | if_statement | for_numeric_statement | for_generic_statement
  | while_statement | repeat_statement | break_statement | label_statement | goto_statement
  | variable_assignment | scoped_variable_declaration | function_definition_statement
  | scoped_function_definition_statement | function_call | empty_statement;

empty_statement = ";";

scoped_function_definition_statement = "local" "function" identifier function_body;

function_definition_statement = "function" function_identifier function_body;
function_identifier = identifier {"." identifier} [":" identifier];

scoped_variable_declaration = "local" scoped_variable_list ["=" expression_list];
scoped_variable_list = scoped_variable {',' scoped_variable};
scoped_variable = identifier [attribute];
attribute = '<' identifier '>';

variable_assignment = variable_list "=" expression_list;
variable_list = variable {"," variable};

goto_statement = "goto" identifier;
label_statement = "::" identifier "::";

break_statement = "break";
repeat_statement = "repeat" [block] "until" expression;
while_statement = "while" expression "do" [block] "end";
for_generic_statement = "for" identifier_list "in" expression_list "do" [block] "end";
for_numeric_statement = "for" identifier "=" expression "," expression ["," expression] "do" [block] "end";

if_statement = "if" expression "then" [block] {"elseif" expression "then" [block]}
  ["else" [block]] "end";

do_statement = "do" [block] "end";

(* expressions *)
expression = nil | boolean | number | string | unary_expression | binary_expression
  | table | vararg_expression | function_definition | prefix_expression;

prefix_expression = variable | function_call | "(" expression ")";
variable = identifier | prefix_expression "." identifier | prefix_expression "[" expression "]";
function_call = prefix_expression [":" identifier] argument_list;
argument_list = string | table | "(" [expression_list] ")";
expression_list = expression {"," expression};

function_definition = "function" function_body;
function_body = "(" [parameter_list] ")" [block] "end";
parameter_list = vararg_expression | identifier_list ["," vararg_expression];
identifier_list = identifier {"," identifier};

vararg_expression = "...";

table = "{" [field_list] "}";
field_list = field {field_separator field} [field_separator];
field = expression | identifier "=" expression | "[" expression "]" "=" expression;
field_separator = "," | ";";

binary_expression = expression binary_operator expression;
binary_operator = "or" | "and" | "==" | "~=" | "<" | ">" | "<=" | ">=" | "|"
  | "~" | "&" | "<<" | ">>" | ".." | "+" | "-" | "*" | "/" | "//" | "%" | "^";

unary_expression = unary_operator expression;
unary_operator = "not" | "#" | "-" | "~";

string = "'" {, inline_character - "'"}, "'" | '"' {, inline_character - '"'}, '"'
  | multiline_start {, character}, multiline_end;
inline_character = ? any character except a newline ?;
character = ? any character ?;

number = decimal | hexadecimal;

hexadecimal = ["-"] ("0x" | "0X"), hexadecimal_numeral [, ("p" | "P"), exponent];
hexadecimal_numeral = hexadecimal_digit {, hexadecimal_digit}
  | hexadecimal_digit {, hexadecimal_digit}, "." {, hexadecimal_digit}
  | {hexadecimal_digit ,} ".", hexadecimal_digit {, hexadecimal_digit};
hexadecimal_digit = ? hexadecimal numeric character ?;

decimal = ["-"] decimal_numeral [, ("e" | "E"), exponent];
decimal_numeral = decimal_digit {, decimal_digit}
  | decimal_digit {, decimal_digit}, "." {, decimal_digit}
  | {decimal_digit ,} ".", decimal_digit {, decimal_digit};
decimal_digit = ? decimal numeric character ?;

exponent = [("+" | "-") ,] decimal_digit {, decimal_digit};

boolean = "false" | "true";

nil = "nil";

identifier = (letter | "_") {, letter | decimal_digit | "_"};
letter = ? case insensitive alphabetic character ?;

(* extras *)
comment = "--" {, inline_character} | "--", multiline_start {, character}, multiline_end;

(* "start" and "end" must have the same number of equal signs ("=") *)
multiline_start = "[" {, "="}, "[";
multiline_end = "]" {, "="}, "]";
```

[extended bnf]: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
[lua manual]: http://www.lua.org/manual/5.4/manual.html#9
