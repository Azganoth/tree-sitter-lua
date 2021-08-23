#include <tree_sitter/parser.h>
#include <cwctype>

namespace {

  using std::iswspace;

  enum TokenType {
    COMMENT,
    STRING_START,
    STRING_CONTENT,
    STRING_END,
  };

  struct Scanner {
  private:
    struct StringLiteral {
      int32_t delimiter;
      bool multiline;
      unsigned delimiter_level;
    };

    StringLiteral last_string;
  public:
    static void skip(TSLexer *lexer) { lexer->advance(lexer, true); }

    static void advance(TSLexer *lexer) { lexer->advance(lexer, false); }

    static bool scan_sequence(TSLexer *lexer, const char *sequence) {
      // Try to match all characters in the given 'sequence'
      for (const char *c = sequence; *c; c++) {
        if (lexer->lookahead == *c) {
          // Consume the character in 'c'
          advance(lexer);
        } else {
          return false;
        }
      }

      return true;
    }

    bool scan_multiline_content(TSLexer *lexer, TokenType type) {
      // Initialize lua multiline content level count
      int start_level = 0;
      int end_level = 0;
      bool has_content = false;
      bool should_end_string = false;

      if (type == COMMENT || type == STRING_START) {
        if (lexer->lookahead == '[') {
          // Consume first appearance '['
          advance(lexer);

          if (lexer->lookahead == '[' || lexer->lookahead == '=') {
            if (type == COMMENT) has_content = true;

            while (lexer->lookahead == '=') {
              // Increment level count
              ++start_level;

              // Consume all '=' characters
              advance(lexer);
            }

            if (lexer->lookahead == '[') {
              // Consume last appearance of '['
              advance(lexer);
              if (type == STRING_START) {
                lexer->result_symbol = type;
                last_string.delimiter_level = start_level;
                last_string.multiline = true;
                return true;
              }
            }
          }
        }
      }

      // continue consuming if content is multiline (has "[[" or derivatives at the start)
      if ((type == COMMENT && has_content) || type == STRING_CONTENT) {
        end_level = type == COMMENT ? start_level : last_string.delimiter_level;

        while (lexer->lookahead != 0) {
          if (lexer->lookahead == ']') {
            lexer->mark_end(lexer);
            advance(lexer);

            if (lexer->lookahead == ']' || lexer->lookahead == '=') {
              while (lexer->lookahead == '=' && end_level > 0) {
                // Decrement level count
                --end_level;

                // Consume all '=' characters
                advance(lexer);
              }

              if (lexer->lookahead == ']' && end_level == 0) {
                lexer->result_symbol = type;
                if (type == COMMENT) {
                  advance(lexer);
                  lexer->mark_end(lexer);

                  return true;
                } else if (has_content) {
                  return true;
                } else {
                  advance(lexer);
                  lexer->mark_end(lexer);
                  should_end_string = true;

                  break;
                }
              }
            }
          }

          if (!should_end_string && lexer->lookahead != 0) {
            has_content = true;
            advance(lexer);
          }
        }
      }

      // it's guaranteed that the next "]...]" string delimiter is valid
      if (type == STRING_END || should_end_string) {
        // we were trying to parse content, but we ended up at the end of the
        // string, so we shouldn't advance anymore
        if (!should_end_string) {
          // consume first ']'
          advance(lexer);

          while (lexer->lookahead != ']')
            advance(lexer);

          // consume last ']'
          advance(lexer);
        }

        lexer->result_symbol = STRING_END;
        return true;
      }

      return false;
    }

    bool scan_content(TSLexer *lexer) {
      bool has_content = false;

      // Loop when isn't new line neither end of file (eof)
      while (lexer->lookahead != '\n' && lexer->lookahead != 0) {
        if (lexer->lookahead == '\\') {
          // Consume '\\'
          advance(lexer);

          if (lexer->lookahead != '\n' && lexer->lookahead != 0) {
            // Consume any character that isn't new line neither end of file (eof)
            advance(lexer);
          } else {
            break;
          }
        } else {
          if (lexer->lookahead == last_string.delimiter) {
            lexer->mark_end(lexer);

            if (has_content) {
              lexer->result_symbol = STRING_CONTENT;
            } else {
              // Consume last appearance of delimiter
              advance(lexer);
              lexer->result_symbol = STRING_END;
              lexer->mark_end(lexer);
            }

            return true;
          } else {
            if (lexer->lookahead != '\n' && lexer->lookahead != 0) {
              // Consume any character that isn't new line neither end of file (eof)
              advance(lexer);
            } else {
              break;
            }
          }
        }

        has_content = true;
      }

      return false;
    }

    bool scan(TSLexer *lexer, const bool *valid_symbols) {
      if (!valid_symbols[STRING_START] &&
          (valid_symbols[STRING_CONTENT] || valid_symbols[STRING_END])) {
        if (last_string.multiline)
          return scan_multiline_content(lexer, valid_symbols[STRING_CONTENT] ? STRING_CONTENT : STRING_END);
        else
          return scan_content(lexer);
      }

      if (valid_symbols[COMMENT] || valid_symbols[STRING_START]) {
        while (iswspace(lexer->lookahead)) {
          skip(lexer);
        }

        // Try to make a short literal string with single/double quotes
        if (lexer->lookahead == '\'' || lexer->lookahead == '"') {
          lexer->result_symbol = STRING_START;

          last_string.delimiter = lexer->lookahead;
          last_string.multiline = false;

          advance(lexer);
          return true;
        }

        // Try to make a comment
        else if (scan_sequence(lexer, "--")) {
          while (iswspace(lexer->lookahead) && lexer->lookahead != '\n' && lexer->lookahead != 0) {
            advance(lexer);
          }

          lexer->result_symbol = COMMENT;

          if (!scan_multiline_content(lexer, COMMENT)) {
            while (lexer->lookahead != '\n' && lexer->lookahead != 0) {
              // Consume any character that isn't new line neither end of file (eof)
              advance(lexer);
            }
          }

          return true;
        }

        else {
          // Try to make a long literal string with double bracket
          return scan_multiline_content(lexer, STRING_START);
        }
      }

      return false;
    }

    unsigned serialize(char* buffer) {
      unsigned i = 0;

      if (3 >= TREE_SITTER_SERIALIZATION_BUFFER_SIZE) return 0;

      buffer[i++] = last_string.delimiter;
      buffer[i++] = last_string.multiline;
      buffer[i++] = last_string.delimiter_level;

      return i;
    }

    void deserialize(const char *buffer, unsigned length) {
      unsigned i = 0;

      if (length == 0) return;

      last_string.delimiter = buffer[i++];
      last_string.multiline = buffer[i++];
      last_string.delimiter_level = buffer[i++];
    }
  };
}

extern "C" {

  void *tree_sitter_lua_external_scanner_create() {
    return new Scanner();
  }

  void tree_sitter_lua_external_scanner_destroy(void *payload) {
    Scanner *scanner = static_cast<Scanner *>(payload);
    delete scanner;
  }

  bool tree_sitter_lua_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols) {
    Scanner *scanner = static_cast<Scanner *>(payload);
    return scanner->scan(lexer, valid_symbols);
  }

  unsigned tree_sitter_lua_external_scanner_serialize(void *payload, char *buffer) {
    Scanner *scanner = static_cast<Scanner *>(payload);
    return scanner->serialize(buffer);
  }

  void tree_sitter_lua_external_scanner_deserialize(void *payload, const char *buffer, unsigned length) {
    Scanner *scanner = static_cast<Scanner *>(payload);
    scanner->deserialize(buffer, length);
  }

}
