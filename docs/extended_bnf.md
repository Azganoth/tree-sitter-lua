# Lua syntax

Complete Lua syntax in [extended BNF][extended bnf], adapted from it’s
[manual][lua manual].

```ebnf
chunk = [block];

block = retstat | stat {stat} [retstat];

(* statements *)
retstat = "return" [explist] [emptstat];

stat = emptstat | varassign | locvardecl | funccall | labstat | gotstat | brstat
  | dostat | whistat | repstat | ifstat | foristat | forstat | funcdefstat | locfuncdefstat;

locfuncdefstat = "local" "function" name funcbody;

funcdefstat = "function" funcname funcbody;
funcname = name {"." name} [":" name];

forstat = "for" namelist "in" explist "do" [block] "end";

foristat = "for" name "=" exp "," exp ["," exp] "do" [block] "end";

ifstat = "if" exp "then" [block] {"elseif" exp "then" [block]} ["else" [block]] "end";

repstat = "repeat" [block] "until" exp;

whistat = "while" exp "do" [block] "end";

dostat = "do" [block] "end";

brstat = "break";

gotstat = "goto" name;
labstat = "::" name "::";

locvardecl = "local" locvarlist ["=" explist];
locvarlist = locvar {',' locvar};
locvar = name [attr];
attr = '<' name '>';

varassign = varlist "=" explist;
varlist = var {"," var};

emptstat = ";";

(* expressions *)
exp = "nil" | "false" | "true" | num | str | varargexp | funcdef | prefixexp | table
  | unexp | binexp;

prefixexp = var | funccall | "(" exp ")";
var = name | prefixexp "[" exp "]" | prefixexp "." name;
funccall = prefixexp [":" name] arglist;
arglist = "(" [explist] ")" | table | str;
explist = exp {"," exp};

funcdef = "function" funcbody;
funcbody = "(" [parlist] ")" [block] "end";
parlist = namelist ["," varargexp] | varargexp;
namelist = name {"," name};

varargexp = "...";

table = "{" [fieldlist] "}";
fieldlist = field {fieldsep field} [fieldsep];
field = "[" exp "]" "=" exp | name "=" exp | exp;
fieldsep = "," | ";";

binexp = exp binop exp;
binop = "or" | "and" | "==" | "~=" | "<" | ">" | "<=" | ">=" | "|" | "~" | "&" | "<<" | ">>"
  | ".." | "+" | "-" | "*" | "/" | "//" | "%" | "^";

unexp = unop exp;
unop = "not" | "#" | "-" | "~";

str = "'" {, inchar - "'"}, "'" | '"' {, inchar - '"'}, '"' | multstart {, char}, multend;
inchar = ? any char except a newline ?;
char = ? any char ?;

num = dec | hex;

hex = ["-"] ("0x" | "0X"), hexnum [, ("p" | "P"), exponent];
hexnum = hexdigit {, hexdigit}
  | hexdigit {, hexdigit}, "." {, hexdigit}
  | {hexdigit ,} ".", hexdigit {, hexdigit};
hexdigit = ? hex numeric char ?;

dec = ["-"] decnum [, ("e" | "E"), exponent];
decnum = decdigit {, decdigit}
  | decdigit {, decdigit}, "." {, decdigit}
  | {decdigit ,} ".", decdigit {, decdigit};
decdigit = ? dec numeric char ?;

exponent = [("+" | "-") ,] decdigit {, decdigit};

name = (letter | "_") {, letter | decdigit | "_"};
letter = ? case insensitive alphabetic char ?;

(* extras *)
comment = "--" {, inchar} | "--", multstart {, char}, multend;

(* "start" and "end" must have the same num of equal signs ("=") *)
multstart = "[" {, "="}, "[";
multend = "]" {, "="}, "]";
```

[extended bnf]: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
[lua manual]: http://www.lua.org/manual/5.4/manual.html#9
