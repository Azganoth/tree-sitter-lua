#!/usr/local/bin/lua
--- shebang

--[[ Comments ]]

-- short comments

--
-- a comment

-- long comments
--[[]]
--[[
a comment
]]
--[=[
Copyright (c) 2022 Ademir José Ferreira Júnior

This source code is licensed under the MIT license found
in the LICENSE file in the root directory of this project.
]=]

--[[ Expressions ]]

-- booleans and nil
print(nil)
print(false)
print(true)

--- integer decimal numbers
print(0)
print(99)
print(-7)
print(-105)

--- integer hexadecimal numbers
print(0x0)
print(0xff)
print(-0xf)
print(-0xfab)

--- float decimal numbers
print(3.1416)
print(314.16e-2)
print(31416.e-4)
print(.31416E1)

--- float hexadecimal numbers
print(0x0.1E)
print(0xBEBADA.)
print(0xA23p-4)
print(0X1.921FB54442D18P+1)

--- short strings
print('')
print("")
print('"You\'re a wizard, Harry!"')
print("\"I'm a what?\"")
print('"Did you put your name into the Goblet of Fire, Harry?" -- Dumbledore asked calmly.')

-- long strings
print([[
Does a thing.

USAGE:
thing [FLAGS]

FLAGS:
    -h, --help       Prints help information
    -V, --version    Prints version information
]])
print([=[
Write-Information
  [-MessageData] <Object>
  [[-Tags] <String[]>]
  [<CommonParameters>]
]=])

-- function definitions
print(function() end)

print(function(x) return x % 2 == 0 end)
print(function(a, b) return a + b end)

print(function(...) return {...} end)
print(function(func, ...) return func(...) end)

-- variable accesses
print(_VERSION)
print(_G._VERSION)
print(_G["_VERSION"])
print(_G._G._G._VERSION)
print(_G["_G"]["_G"]["_VERSION"])

-- function calls
print(type(_G))
print(type'')
print(type"")
print(type[[]])
print(type{})

print(io.output())
print(io.output():read())

-- tables
print({
  "x",
  "y";
  {},
  rawlen"four"
})
print({
  [54] = _VERSION,
  ["version"] = _VERSION,
  [_VERSION] = 54,
  _VERSION = 54,
})

-- unary expressions
print(not true)
print(#"four")
print(-#"-five")
print(~0)

-- binary expressions
print(1*2 + 4/2)
print(5^2 % 5)
print(5 - -5 == 0 and 1 or 0)

--[[ Statements ]]

-- variable assignments
x, y = 5.25, -3.5
r, g, b, a = 0xf2, 0x3b, 0x2c

-- local variable declarations
local pi<const> = math.pi

local circumference, nothing<close> = 2 * pi * 24

-- function calls (as statement)
print(_G)
print''
print""
print[[]]
print{}

io.output()
io.output():read()

-- labels
goto next_line
:: next_line ::

-- simple conditional structures
if true then end
if false then else end
if false then elseif true then end
if false then elseif false then elseif false then elseif true then else end

temperature = math.random(0, 50)
if temperature > 25 then
  print"is hot"
end

n = math.random(0, 100)
if n % 2 == 0 then
  print"is even"
else
  print"is odd"
end

weathers = {"rain", "windy"}
weather = weathers[math.random(2)]
if weather == "rain" then
  print"just stay home, it's raining"
elseif weather == "windy" then
  print"just stay home, it's too windy"
end

average_note = math.random(0, 10)
if average_note >= 7.5 then
  print"A"
elseif average_note >= 6 then
  print"B"
elseif average_note >= 4 then
  print"C"
elseif average_note >= 2 then
  print"D"
else
  print"E"
end

-- conditional loop structures
while false do end
repeat until true

ten = 10
while ten > 0 do
  ten = ten - 1
end

repeat
  ten = ten + 1
until ten < 10

-- numeric for loops
for i = 0, 0 do end
for i = 0, 0, 1 do end

zero = 0
for i = zero, 10 do
  zero = i
end

for i = zero, 0, -1 do
  zero = i
end

-- generic for loops
for i, v in ipairs({}) do end

weathers = {"rain", "sleet", "snow", "freezing fog", "sunny", "cloudy", "ice pellets", "windy"}
for _, weather in ipairs(weathers) do
  print(weather)
end

-- scope blocks
do end

scope_level = 0
do
  local scope_level = scope_level + 1
  do
    local scope_level = scope_level + 1
  end
end

-- function definitions (syntactic sugar)
function noop() end

function is_even(x) return x % 2 == 0 end
function sum(a, b) return a + b end

function make_list(...) return {...} end
function apply(func, ...) return func(...) end

function _G.noop() end
function _G._G.noop() end
function _G._G:noop() end

-- local function definitions (syntactic sugar)
local function _noop() end

local function _is_even(x) return x % 2 == 0 end
local function _sum(a, b) return a + b end

local function _make_list(...) return {...} end
local function _apply(func, ...) return func(...) end
