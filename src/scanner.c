#include <tree_sitter/parser.h>

enum TokenType
{
  SHORT_SQ_STRING_CONTENT,
  SHORT_DQ_STRING_CONTENT,
  LONG_STRING,
  LONG_COMMENT,
};

// helpers
const char EOF = 0;

static void consume(TSLexer *lexer)
{
  lexer->advance(lexer, false);
}
static void skip(TSLexer *lexer)
{
  lexer->advance(lexer, true);
}

// scanner
void *tree_sitter_lua_external_scanner_create()
{
  return NULL;
}

void tree_sitter_lua_external_scanner_destroy(void *payload)
{
}

unsigned tree_sitter_lua_external_scanner_serialize(
    void *payload,
    char *buffer)
{
  return 0;
}

void tree_sitter_lua_external_scanner_deserialize(
    void *payload,
    const char *buffer,
    unsigned length)
{
}

static bool is_not_new_line_or_eof(TSLexer *lexer)
{
  return lexer->lookahead != '\n' && lexer->lookahead != EOF;
}

bool tree_sitter_lua_external_scanner_scan(
    void *payload,
    TSLexer *lexer,
    const bool *valid_symbols)
{
  if (valid_symbols[SHORT_SQ_STRING_CONTENT] ||
      valid_symbols[SHORT_DQ_STRING_CONTENT])
  {
    const bool is_single_quote = valid_symbols[SHORT_SQ_STRING_CONTENT];
    const char end_quote = is_single_quote ? '\'' : '"';

    // try to consume almost anything
    if (lexer->lookahead != end_quote && is_not_new_line_or_eof(lexer))
    {
      do
      {
        // try to consume a backslash
        if (lexer->lookahead == '\\')
        {
          consume(lexer);

          // try to consume almost anything
          if (is_not_new_line_or_eof(lexer))
          {
            consume(lexer);
          }
        }
        else
        {
          consume(lexer);
        }
        // consume almost everything
      } while (lexer->lookahead != end_quote && is_not_new_line_or_eof(lexer));

      lexer->result_symbol = is_single_quote
                                 ? SHORT_SQ_STRING_CONTENT
                                 : SHORT_DQ_STRING_CONTENT;
      return true;
    }
  }
  else if (valid_symbols[LONG_STRING] || valid_symbols[LONG_COMMENT])
  {
    // try to consume a first opening bracket
    if (lexer->lookahead == '[')
    {
      consume(lexer);

      // consume any level delimiters, and store how many there are
      unsigned int level_count = 0;
      while (lexer->lookahead == '=')
      {
        consume(lexer);
        level_count += 1;
      }

      // try to consume the last opening bracket
      if (lexer->lookahead == '[')
      {
        consume(lexer);

        // consume almost everything
        while (lexer->lookahead != EOF)
        {
          // try to consume a first closing bracket
          if (lexer->lookahead == ']')
          {
            consume(lexer);

            // try to consume every level delimiters
            unsigned int current_level;
            for (current_level = level_count;
                 current_level > 0 && lexer->lookahead == '=';
                 current_level -= 1)
            {
              consume(lexer);
            }

            // try to consume the last closing bracket if all levels are consumed
            if (current_level == 0 && lexer->lookahead == ']')
            {
              consume(lexer);

              lexer->result_symbol = valid_symbols[LONG_STRING]
                                         ? LONG_STRING
                                         : LONG_COMMENT;
              return true;
            }
          }
          else
          {
            consume(lexer);
          }
        }
      }
    }
  }

  return false;
}
