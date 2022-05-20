# Lua syntax

Complete Lua syntax in [extended BNF][extended bnf], as in it’s
[own manual][lua manual].

```ebnf
chunk ::=
  block

block ::=
  {stat} [retstat]

stat ::=
  ';' | varlist '=' explist | functioncall | label | break | goto Name |
  do block end |
  while exp do block end |
  repeat block until exp |
  if exp then block {elseif exp then block} [else block] end |
  for Name '=' exp ',' exp [',' exp] do block end |
  for namelist in explist do block end |
  function funcname funcbody |
  local function Name funcbody |
  local namelist ['=' explist]

retstat ::=
  return [explist] [';']

label ::=
  '::' Name '::'

funcname ::=
  Name {'.' Name} [':' Name]

varlist ::=
  var {, var}

var ::=
  Name | prefixexp '[' exp ']' | prefixexp '.' Name

namelist ::=
  Name {',' Name}

explist ::=
  exp {',' exp}

exp ::=
  nil | false | true | Numeral | LiteralString | '...' | functiondef |
  prefixexp | tableconstructor | exp binop exp | unop exp

prefixexp ::=
  var | functioncall | '(' exp ')'

functioncall ::=
  prefixexp args | prefixexp ':' Name args

args ::=
  '(' [explist] ')' | tableconstructor | LiteralString

functiondef ::=
  function funcbody

funcbody ::=
  '(' [parlist] ')' block end

parlist ::=
  namelist [',' '...'] | '...'

tableconstructor ::=
  '{' [ fieldlist ] '}'

fieldlist ::=
  field {fieldsep field} [fieldsep]

field ::=
  '[' exp ']' '=' exp | Name '=' exp | exp

fieldsep ::=
  ',' | ';'

binop ::=
  '+' | '-' | '*' | '/' | '//' | '^' | '%' |
  '&' | '~' | '|' | '>>' | '<<' | '..' |
  '<' | '<=' | '>' | '>=' | '==' | '~=' |
  not | or

unop ::=
   '-' | not | '#' | '~'
```

[extended bnf]: https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
[lua manual]: http://www.lua.org/manual/5.3/manual.html#9
