#include "eof_parser.h"
#include "platform.h"
#include "../rnd.h"

#include <stdlib.h>
#include <sstream>
#include <list>
#include <algorithm>
#include <assert.h>

#ifdef __PLATFORM_WIN32__
#include <iomanip>
#else
#define _GNU_SOURCE
#include <time.h>
#endif

namespace sys {
	namespace eof {

	const std::string EToken::TYPE_NAMES[] = {
			"NULL",
			"id",
			"value",
			"field",
			"defn",
			"string",
			"bitfield",
			"time",
			"bool",
			"enum"
	};

	///////////////////////////////////////////////////////////////////////////

	int _PEValue::asInt() const
	{
		if (t == INTEGER) return u.i;
		return (int)u.f;
	}

	float _PEValue::asFloat() const
	{
		if (t == FLOAT) return u.f;
		return (float)u.i;
	}

	void _PEValue::fromInt(int i)
	{
		t = INTEGER;
		u.i = i;
	}

	void _PEValue::fromFloat(float f)
	{
		t = FLOAT;
		u.f = f;
	}

	///////////////////////////////////////////////////////////////////////////

	EValueFloat::EValueFloat(float f) : m_value(f)
	{
		// nothing
	}

	EValueFloat::~EValueFloat()
	{
		// nothing
	}

	PEValue EValueFloat::value()
	{
		PEValue pg;
		pg.fromFloat(m_value);

		return pg;
	}

	EValue* EValueFloat::copy()
	{
		return new EValueFloat(m_value);
	}

	///////////////////////////////////////////////////////////////////////////

	EValueInt::EValueInt(int i) : m_value(i)
	{
		// nothing
	}

	EValueInt::~EValueInt()
	{
		// nothing
	}

	PEValue EValueInt::value()
	{
		PEValue pg;
		pg.fromInt(m_value);

		return pg;
	}

	EValue* EValueInt::copy()
	{
		return new EValueInt(m_value);
	}

	///////////////////////////////////////////////////////////////////////////

	EValueFunc::EValueFunc() :
		m_param1(NULL), m_param2(NULL)
	{
		// nothing
	}

	EValueFunc::~EValueFunc()
	{
		delete m_param1;
		delete m_param2;
	}

	EValueFuncNoArgs::EValueFuncNoArgs(pEValueFuncNoArgs func) : m_func(func)
	{
		// nothing
	}

	EValueFuncOneArg::EValueFuncOneArg(pEValueFuncOneArg func, EValue* param) : m_func(func)
	{
		m_param1 = param;
	}

	EValueFuncOneArg::EValueFuncOneArg(pEValueFuncOneArg func) : m_func(func)
	{
	}

	EValueFuncTwoArgs::EValueFuncTwoArgs(pEValueFuncTwoArgs func, EValue* p1, EValue* p2) : m_func(func)
	{
		m_param1 = p1;
		m_param2 = p2;
	}

	EValueFuncTwoArgs::EValueFuncTwoArgs(pEValueFuncTwoArgs func) : m_func(func)
	{

	}

	PEValue EValueFuncNoArgs::value()
	{
		return m_func();
	}

	PEValue EValueFuncNoArgs::value(EValue* v1, EValue* v2)
	{
		return m_func();
	}

	PEValue EValueFuncOneArg::value()
	{
		return m_func(m_param1);
	}

	PEValue EValueFuncOneArg::value(EValue* v1, EValue* v2)
	{
		return m_func(v1);
	}

	PEValue EValueFuncTwoArgs::value()
	{
		return m_func(m_param1, m_param2);
	}

	PEValue EValueFuncTwoArgs::value(EValue* v1, EValue* v2)
	{
		return m_func(v1, v2);
	}

	EValue* EValueFuncTwoArgs::copy()
	{
		return new EValueFuncTwoArgs(m_func, m_param1, m_param2);
	}

	int EValueFuncTwoArgs::nArgs() const
	{
		return 2;
	}

	EValue* EValueFuncOneArg::copy()
	{
		return new EValueFuncOneArg(m_func, m_param1);
	}

	int EValueFuncOneArg::nArgs() const
	{
		return 1;
	}

	EValue* EValueFuncNoArgs::copy()
	{
		return new EValueFuncNoArgs(m_func);
	}

	int EValueFuncNoArgs::nArgs() const
	{
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////

	EString::EString() : EToken(E_STRING)
	{
		// nothing
	}

	EString::EString(const std::string& sz) : EToken(E_STRING), m_string(sz)
	{
		// nothing
	}

	EString::~EString()
	{
		// nothing
	}

	std::string EString::value()
	{
		return m_string;
	}

	///////////////////////////////////////////////////////////////////////////

	EBoolean::EBoolean(bool b) : EToken(E_BOOL), m_bool(b)
	{
		// nothing
	}

	EBoolean::~EBoolean()
	{
		// nothing
	}

	bool EBoolean::value()
	{
		return m_bool;
	}

	///////////////////////////////////////////////////////////////////////////

	ETime::ETime() : EToken(E_TIME)
	{
		time(&m_time);
	}

	ETime::ETime(time_t t) : EToken(E_TIME), m_time(t)
	{
		// nothing
	}

	ETime::~ETime()
	{
		// nothing
	}

	time_t ETime::value()
	{
		return m_time;
	}

	///////////////////////////////////////////////////////////////////////////

	const unsigned int EBitField::BIT_MAX = 32;

	EBitField::EBitField() : EToken(E_BITFIELD), m_field(0)
	{
		// nothing
	}

	EBitField::EBitField(unsigned int field) : EToken(E_BITFIELD), m_field(field)
	{
		// nothing
	}

	EBitField::~EBitField()
	{
		// nothing
	}

	unsigned int EBitField::value()
	{
		return m_field;
	}

	void EBitField::set(unsigned int bit)
	{
		if (bit < BIT_MAX) {
			m_field |= B1(bit);
		}
	}

	bool EBitField::isSet(unsigned int bit) const
	{
		if (bit < BIT_MAX) {
			return ((m_field & B1(bit)) != 0);
		}

		return false;
	}

	///////////////////////////////////////////////////////////////////////////

	EDeclKeyValue::EDeclKeyValue(const std::string& k, const std::string& v) : m_key(k), m_value(v)
	{
	}

	EDeclKeyValue::~EDeclKeyValue()
	{
	}

	std::string EDeclKeyValue::key() const
	{
		return m_key;
	}

	std::string EDeclKeyValue::value() const
	{
		return m_value;
	}

	std::string EDeclKeyValue::text() const
	{
		std::ostringstream oss;

		oss << EOF_DECL_OPEN;
		oss << m_key;
		oss << EOF_DECL_DELIM;
		oss << m_value;
		oss << EOF_DECL_CLOSE;

		return oss.str();
	}

	///////////////////////////////////////////////////////////////////////////

	EDeclSection::EDeclSection(const std::string& p) : m_path(p)
	{
	}

	EDeclSection::~EDeclSection()
	{
	}

	std::string EDeclSection::path() const
	{
		return m_path;
	}

	std::string EDeclSection::text() const
	{
		std::ostringstream oss;

		oss << EOF_DECL_OPEN;
		oss << m_path;
		oss << EOF_DECL_CLOSE;

		return oss.str();
	}

	///////////////////////////////////////////////////////////////////////////

	EDeclNull::EDeclNull()
	{
	}

	EDeclNull::~EDeclNull()
	{
	}

	std::string EDeclNull::text() const
	{
		std::ostringstream oss;

		oss << EOF_DECL_OPEN;
		oss << EOF_DECL_DELIM;
		oss << EOF_DECL_CLOSE;

		return oss.str();
	}

	///////////////////////////////////////////////////////////////////////////

	EDefinition::EDefinition(const std::string& sz) : m_string(sz)
	{
	}

	EDefinition::~EDefinition()
	{
	}

	std::string EDefinition::text() const
	{
		std::ostringstream oss;

		oss << EOF_DEFN_OPEN;
		oss << m_string;
		oss << EOF_DEFN_CLOSE;

		return oss.str();
	}

	///////////////////////////////////////////////////////////////////////////

	EShortString::EShortString()
	{
		clear();
	}

	EShortString::EShortString(const std::string& sz)
	{
		assign(sz);
	}

	EShortString::EShortString(const char* sz)
	{
		assign(sz);
	}

	EShortString::~EShortString()
	{
		// nothing
	}

	bool EShortString::isshort(unsigned char c)
	{
		return (((c >= 'A') && (c <= 'Z')) ||
				((c >= 'a') && (c <= 'z')) ||
				((c >= '0') && (c <= '9')) ||
				(c == '_'));
	}

	const char* EShortString::str() const
	{
		return m_string;
	}

	unsigned int EShortString::length() const
	{
		return m_length;
	}

	EShortString& EShortString::operator=(const EShortString& rhs)
	{
		if (this != &rhs) {
			clear();
			unsigned int len = rhs.length();

			for (unsigned int i = 0; i < len; i++) {
				char c = rhs.at(i);

				if (isshort(c)) {
					m_string[m_length++] = c;
				}
			}
		}

		return *this;
	}

	bool EShortString::operator==(const EShortString& rhs) const
	{
		return compare(rhs);
	}

	bool EShortString::operator<(const EShortString &rhs) const
	{
		unsigned int len = std::min(m_length, rhs.length());

		for (unsigned int i = 0; i < len; i++) {
			if (m_string[i] != rhs.at(i)) {
				return m_string[i] < rhs.at(i);
			}
		}

		return false;
	}

	// compares this short string to the given string based on the
	// given case sensitivity.  true is returned if they are equal
	bool EShortString::compare(const std::string& sz, bool caseSensitive) const
	{
		if (sz.length() != m_length) return false;

		for (unsigned int i = 0; i < m_length; i++) {
			if (caseSensitive) {
				if (m_string[i] != sz.at(i)) return false;
			} else {
				if (tolower(m_string[i]) != tolower(sz.at(i))) return false;
			}
		}

		return true;
	}

	bool EShortString::compare(const char* sz, bool caseSensitive) const
	{
		if (strlen(sz) != m_length) return false;

		for (unsigned int i = 0; i < m_length; i++) {
			if (caseSensitive) {
				if (m_string[i] != sz[i]) return false;
			} else {
				if (tolower(m_string[i]) != tolower(sz[i])) return false;
			}
		}

		return true;
	}

	bool EShortString::compare(const EShortString& sz, bool caseSensitive) const
	{
		if (sz.length() != m_length) return false;

		for (unsigned int i = 0; i < m_length; i++) {
			if (caseSensitive) {
				if (m_string[i] != sz.at(i)) return false;
			} else {
				if (tolower(m_string[i]) != tolower(sz.at(i))) return false;
			}
		}

		return true;
	}

	bool EShortString::compare(char c, bool caseSensitive) const
	{
		if (m_length != 1) return false;

		if (caseSensitive) {
			return (m_string[0] == c);
		} else {
			return (tolower(m_string[0]) == tolower(c));
		}
	}

	// assigns this short string to the given value
	void EShortString::assign(const std::string& sz)
	{
		clear();
		unsigned int len = std::min(sz.length(), EOF_SHORTSTRING_LEN);

		for (unsigned int i = 0; i < len; i++) {
			char c = sz.at(i);
			if (isshort(c)) {
				m_string[m_length++] = c;
			}
		}
	}

	void EShortString::assign(const char* sz)
	{
		clear();
		unsigned int len = std::min(strlen(sz), EOF_SHORTSTRING_LEN);

		for (unsigned int i = 0; i < len; i++) {
			char c = sz[i];
			if (isshort(c)) {
				m_string[m_length++] = c;
			}
		}
	}

	void EShortString::assign(const EShortString& sz)
	{
		operator=(sz);
	}

	void EShortString::assign(char c, unsigned int n)
	{
		clear();
		m_length = std::min(n, EOF_SHORTSTRING_LEN);

		if (isshort(c)) {
			for (unsigned int i = 0; i < m_length; i++) {
				m_string[i] = c;
			}
		}
	}

	void EShortString::clear()
	{
		memset(m_string, 0, EOF_SHORTSTRING_LEN + 1);
		m_length = 0;
	}

	bool EShortString::empty() const
	{
		return (m_length == 0);
	}

	EShortString& EShortString::operator+=(const EShortString& rhs)
	{
		unsigned int len = std::min(rhs.length(), EOF_SHORTSTRING_LEN - m_length);

		for (unsigned int i = 0; i < len; i++) {
			m_string[m_length + i] = rhs.at(i);
		}

		m_length += len;

		return *this;
	}

	EShortString& EShortString::operator+=(char rhs)
	{
		if (m_length < EOF_SHORTSTRING_LEN) {
			m_string[m_length++] = rhs;
		}

		return *this;
	}

	char& EShortString::operator[](int n)
	{
		assert((n > 0) && (n < m_length));
		return m_string[n];
	}

	const char& EShortString::operator[](int n) const
	{
		assert((n > 0) && (n < m_length));
		return m_string[n];
	}

	char EShortString::at(int n) const
	{
		if ((n > 0) && (n < m_length)) {
			return m_string[n];
		}

		return 0;
	}

	///////////////////////////////////////////////////////////////////////////

	const EParser::EBuiltinTable EParser::FuncTable[] = {
		{ "RAND",	new EValueFuncNoArgs(func_random_rand) },
		{ "RNDN",	new EValueFuncNoArgs(func_random_rndn) },
		{ "RNDG",	new EValueFuncNoArgs(func_random_rndg) },
		{ "RRNG",	new EValueFuncTwoArgs(func_random_range) },
		{ "ROLL",	new EValueFuncTwoArgs(func_random_roll) },
		{ "PRLN",	new EValueFuncOneArg(func_random_perlin) },
		{ "ADD",	new EValueFuncTwoArgs(func_math_add) },
		{ "MULT",	new EValueFuncTwoArgs(func_math_mult) },
		{ "DIV",	new EValueFuncTwoArgs(func_math_div) },
		{ "SUB",	new EValueFuncTwoArgs(func_math_sub) },
		{ "SIN",	new EValueFuncOneArg(func_math_sin) },
		{ "COS",	new EValueFuncOneArg(func_math_cos) },
		{ "LOG",	new EValueFuncOneArg(func_math_log) },
		{ "SQR",	new EValueFuncOneArg(func_math_sqr) },
		{ "SQRT",	new EValueFuncOneArg(func_math_sqrt) }
	};
	// function not found.  functions first argument will be used as value.

	const EParser::EBuiltinTable EParser::ConstTable[] = {
		{ "PI", new EValueFloat(3.14159265359f) },
		{ "E", new EValueFloat(2.71828182846f) }
	};

	///////////////////////////////////////////////////////////////////////////

	EParser::EParser(const char* filename)
	{
#ifdef __PLATFORM_WIN32__
		fopen_s(&m_fp, filename, "r");
#else
		m_fp = fopen(filename, "r");
#endif

		resetContext();
		char *buffer;

		if (m_fp) {
			std::fseek(m_fp, 0, SEEK_END);
			size_t cap = std::min((size_t)EOF_PARSER_FILEMAX, (size_t)std::ftell(m_fp));
			std::rewind(m_fp);

			buffer = new char[cap];
			fread(buffer, 1, cap, m_fp);
			m_contents.assign(buffer);
			delete[] buffer;
		}
	}

	EParser::EParser(const std::string& sz) : m_fp(NULL)
	{
		resetContext();

		m_contents.assign(sz);
	}

	EParser::~EParser()
	{
		if (m_fp) {
			fclose(m_fp);
		}
	}

	void EParser::resetContext()
	{
		m_context.pos = 0;
		m_context.start = 0;

		m_context.pdepth = 0;
		m_context.sdepth = 0;
		m_context.cdepth = 0;

		m_context.flags = 0;
	}

	bool EParser::ishex(unsigned char c)
	{
		return (((c >= 'A') && (c <= 'F')) ||
				((c >= 'a') && (c <= 'f')) ||
				((c >= '0') && (c <= '9')));
	}

	bool EParser::isbinary(unsigned char c)
	{
		return ((c == '0') || (c == '1'));
	}

	unsigned char EParser::specialEscape(unsigned char c)
	{
		unsigned char ret = c;

		switch (c) {
		default: break;
		case 'n': ret = '\n'; break;
		case 'r': ret = '\r'; break;
		case 't': ret = '\t'; break;
		case '0': ret = '\0'; break;
		case 'v': ret = '\v'; break;
		case 'f': ret = '\f'; break;
		case 'a': ret = '\a'; break;
		}

		return ret;
	}

	pEStatement EParser::parseDecl()
	{
		std::string key, value;
		EStatement* statement = NULL;

		// true if we are parsing the first of a declaration
		bool first = true;
		bool escaped = false;

		// we can assume we are inside a decl at this point
		for (; m_context.pos < m_contents.length(); m_context.pos++) {
			unsigned char c = m_contents[m_context.pos];

			if (c == EOF_COMMENT_CHAR) {
				if ((first) || (!escaped)) {
					m_context.flags |= E_PARSE_COMMENT;
					continue;
				} else {
					value += c;
					escaped = false;
				}
			} else if (m_context.flags & E_PARSE_COMMENT) {
				// in comment
				if (c == '\n') { m_context.flags &= ~E_PARSE_COMMENT; }
				continue;
			} else if (isspace(c)) {
				if (first) continue;
				value += c;
			} else if ((!first) && (c == EOF_ESCAPE_CHAR)) {
				if (escaped) {
					// we have an escaped backslash
					value += c;
					escaped = false;
				} else {
					escaped = true;
				}
			} else if (c == EOF_DECL_CLOSE) {
				if (escaped) {
					value += c;
					escaped = false;
				} else {
					m_context.sdepth--;

					if (m_context.sdepth == 0) {
						m_context.flags &= ~E_PARSE_IN_SQUARE;
					}

					if (first) {
						if (key.size() != 0) {
							statement = new EDeclSection(key);
						} else {
							printf("warning: empty section, ignored.\n");
						}
					} else {
						if (key.size() == 0) {
							statement = new EDeclNull();
						} else {
							statement = new EDeclKeyValue(key, value);
						}
					}
					break;
				}
			} else if (c == EOF_DECL_OPEN) {
				if (first) {
					printf("error: syntax error at 0x%04x: nested declarations not allowed\n", m_context.pos);
					break;
				} else {
					value += c;
				}
			} else if (c == EOF_DECL_DELIM) {
				if (first) {
					first = false;
				} else {
					value += c;
				}
			} else {
				// legal character
				if (escaped) {
					c = specialEscape(c);

					escaped = false;
				}

				if (first) {
					key += c;
				} else {
					value += c;
				}
			}
		}

//		printf("decl[%d]:=%s\n", m_context.sdepth, statement.c_str());
		return statement;
	}

	pEStatement EParser::parseDefn()
	{
		EStatement* statement = NULL;

		std::string sstr;

		// we can assume we are inside a decl at this point
		for (; m_context.pos < m_contents.length(); m_context.pos++) {
			unsigned char c = m_contents[m_context.pos];

			if (c == '#') {
				m_context.flags |= E_PARSE_COMMENT;
				continue;
			} else if (m_context.flags & E_PARSE_COMMENT) {
				// in comment
				if (c == '\n') { m_context.flags &= ~E_PARSE_COMMENT; }
				continue;
			} else if ((c == EOF_DOUBLE_QUOTE) || (c == EOF_SINGLE_QUOTE)) {
				// when we return from parse quote, we will be past the string

				sstr += EOF_DOUBLE_QUOTE;
				sstr.append(parseQuote(true));
				sstr += EOF_DOUBLE_QUOTE;

			} else if (isspace(c)) {
				continue;
			} else if (c == EOF_DEFN_CLOSE) {
				m_context.cdepth--;

				if (m_context.cdepth == 0) {
					m_context.flags &= ~E_PARSE_IN_SQUARE;
				}
				break;
			} else if (c == EOF_DEFN_OPEN) {
				m_context.pos++;
				m_context.cdepth++;

				EStatement* child = parseDefn();
				if (child) {
					sstr.append(child->text());
					delete child;
				}
			} else {
				// acceptable token character
				sstr += c;
			}
		}

		if (sstr.size() == 0) {
			printf("warning: empty definition, ignored.\n");
		} else {
			statement = new EDefinition(sstr);
		}

//		printf("defn[%d]:=%s\n", m_context.cdepth, statement.c_str());
		return statement;
	}
	
	std::string EParser::parseQuote(bool keepslashes)
	{
		bool escaped = false;
		std::string statement;

		unsigned char c = m_contents[m_context.pos++];

		if (m_context.flags & E_PARSE_IN_STRING) {
			return statement;
		} else if (c == EOF_DOUBLE_QUOTE) {
			m_context.flags |= E_PARSE_IN_DQUOTE;
		} else if (c == EOF_SINGLE_QUOTE) {
			m_context.flags |= E_PARSE_IN_SQUOTE;
		} else {
			printf("warning: string type expected, but no string found.\n");
			return statement;
		}

		// we can assume we are inside a decl at this point
		for (; m_context.pos < m_contents.length(); m_context.pos++) {
			c = m_contents[m_context.pos];

			if (c == EOF_ESCAPE_CHAR) {
				if (escaped) {
					// we have an escaped backslash
					if (keepslashes) statement += EOF_ESCAPE_CHAR;
					statement += c;
					escaped = false;
				} else {
					escaped = true;
				}
			} else if (escaped) {
				// we just saw an escaped character, is it something special?
				if (keepslashes) {
					statement += EOF_ESCAPE_CHAR;
					statement += c;
				} else {
					statement += specialEscape(c);
				}

				escaped = false;
			} else if ((m_context.flags & E_PARSE_IN_DQUOTE) &&
					   (c == EOF_DOUBLE_QUOTE)) {
				m_context.flags &= ~E_PARSE_IN_DQUOTE;
				break;
			} else if ((m_context.flags & E_PARSE_IN_SQUOTE) &&
					   (c == EOF_SINGLE_QUOTE)) {
				m_context.flags &= ~E_PARSE_IN_SQUOTE;
				break;
			} else {
				statement += c;
			}
		}

//		printf("quote:=%s\n", statement.c_str());
		return statement;
	}

	bool EParser::verifyShortString(const std::string& sz) const
	{
		for (int i = 0; i < (int)sz.length(); i++) {
			if (!EShortString::isshort(sz.at(i))) return false;
		}

		return true;
	}

	bool EParser::caseInsensitiveEqual(const std::string& sz, const std::string& buf) const
	{
		int len = (int)std::min(sz.length(), buf.length());

		for (int i = 0; i < len; i++) {
			if (tolower(sz.at(i)) != tolower(buf.at(i))) return false;
		}

		return true;
	}

	pEToken EParser::isBitvalue(const std::string& val) const
	{
		unsigned int bf;

		if ((val.length() > 2) &&
			(val.at(0) == '0') &&
			(tolower(val.at(1)) == 'x'))
		{
			// parse has hex number
			for (int i = 2; i < val.length(); i++) {
				if (!ishex(val.at(i))) return static_cast<pEToken>(0);
			}

			EValue *ev = parseLiteral(val);

			if (ev) {
				bf = (unsigned int)ev->value().asInt();
				delete ev;

				return new EBitField(bf);
			}
		} else if ((val.length() >= 8) && (val.length() <= 32)) {
			bf = 0;

			// parse as 0 or 1
			for (int i = 0; i < val.length(); i++) {
				if (!isbinary(val.at(i))) return static_cast<pEToken>(0);

				if (val.at(val.length() - (i + 1)) == '1') {
					bf |= B1(i);
				}
			}

			return new EBitField(bf);
		}

		return static_cast<pEToken>(0);
	}

	pEToken EParser::parseField()
	{
		return static_cast<pEToken>(0);
	}

	pEToken EParser::parseValue(const std::string& val)
	{
		std::list<EValue*> values;
		std::list<EValue*> func;
		std::string buf;
		char c;

		// here we loop through the string, splitting on (), characters and
		// putting functions in one stack and literal values in another
		for (int i = 0; i <= val.length(); i++) {
			if (i == val.length()) {
				c = EOF_END_OF_STR;
			} else {
				c = val.at(i);
			}

			if ((c == EOF_END_OF_STR) ||
				(c == EOF_FIELD_OPEN) ||
				(c == EOF_FIELD_CLOSE) ||
				(c == EOF_TOKEN_DELIM)) {
				if (!buf.empty()) {
					static const int sBUILTIN_SIZE = sizeof(FuncTable) / sizeof(EBuiltinTable);
					bool found = false;

					for (int x = 0; x < sBUILTIN_SIZE; x++) {
						if (FuncTable[x].name.compare(buf) == 0) {
							found = true;
							EValue* fptr = dynamic_cast<EValue*>(FuncTable[x].token);

							if (fptr) {
								func.push_front(fptr->copy());

								if (func.size() > EOF_FUNC_DEPTH) {
									func.pop_front();
									printf("warning: recursive function depth exceeded - skipping");
									i = val.length() + 1;	// exit the loop
								}
							}
							break;
						}
					}

					if (!found) {
						EValue* pl = parseLiteral(buf);

						if (pl) {
							values.push_front(pl);
						}
					}

					buf.clear();
				}
			} else {
				buf += c;
			}
		}

		// this is basically shunting yard, we have our functions in reverse order
		// in the func list, and our values in reverse order in the values list.
		// now, we pop a function, and pop the off the values list the number of
		// values equal to the number of function arguments.  Then, we pop the result
		// to the values list.

		// Continue this process until the function list is empty, if exactly one value
		// is on the values list, the process is complete and valid with that value being
		// result
		while (func.size() > 0) {
			EValueFunc* f = dynamic_cast<EValueFunc*>(func.front());
			func.pop_front();

			if (f) { // just to be safe
				int n = f->nArgs();

				if (values.size() < n) {
					printf("error: not enough arguments for function\n");
					func.clear();
					break;
				} else {
					EValue *p1, *p2;

					// for now we only support 2 argument functions, otherwise we will have
					// to get creative about the ways we can pass argument parameters
					switch (n) {
					default:
					case 0:
						// nothing
						break;
					case 1:
						p1 = values.front();
						values.pop_front();

						f->args(p1);
						break;
					case 2:
						p2 = values.front();	// arg2 first
						values.pop_front();
						p1 = values.front();
						values.pop_front();

						f->args(p1, p2);
						break;
					}

					values.push_front(f);
				}
			}
		}

		if (values.size() == 1) {
			return values.front();
		} else {
			printf("error: unused arguments in expression\n");
		}

		return static_cast<pEToken>(0);
	}

	EValue* EParser::parseLiteral(const std::string& val) const
	{
		// digit : ([0-9]*(\.[0-9]*)?)
		//   hex : (0x[0-9A-Fa-f]+)
		// value : (([+-])?digit|hex)
		bool hex = false;
		int i = 0;

		unsigned int u;
		int			 s;
		float		 f;

		// first, check if the literal is a known constant
		static const int sBUILTIN_SIZE = sizeof(ConstTable) / sizeof(EBuiltinTable);
		for (int x = 0; x < sBUILTIN_SIZE; x++) {
			if (FuncTable[x].name.compare(val) == 0) {
				EValue* v = dynamic_cast<EValue*>(FuncTable[x].token);

				if (v) {
					return v->copy();
				}
			}
		}

		if (val.length() > 0) {
			if ((val.at(0) == '+') || (val.at(0) == '-')) {
				i++;
			}

			if ((val.length() > (2 + i)) &&
				(val.at(0 + i) == '0') &&
				(tolower(val.at(1 + i)) == 'x')) {
				hex = true;
			}

			if (hex) {
				// hex value
				int n = sscanf_s(val.c_str(), "%x", &u);
				if (n == 1) return new EValueInt((int)u);
			} else if (val.find('.') == std::string::npos) {
				// int
				int n = sscanf_s(val.c_str(), "%d", &s);
				if (n == 1) return new EValueInt(s);
			} else {
				// float
				int n = sscanf_s(val.c_str(), "%f", &f);
				if (n == 1) return new EValueFloat(f);
			}
		}

		return static_cast<EValue*>(0);
	}

	void EParser::parseTokens(const std::string& defn)
	{
		static const std::string sTRUE("true");
		static const std::string sFALSE("false");

		// we want to parse each definition and split it into parts delimited by ','.
		// we need to be mindful of quoted strings and nested defintions.  remember
		// that we already have the definition as it is with extra whitespace removed.
		// once split, we try to guess the type from a set of known values.

		// shortstring : ([A-Za-z0-9_]*)
		//       digit : ([0-9]*(\.[0-9]*)?)
		//         hex : (0x[0-9A-Fa-f]+)
		//     iso8601 : ((-?(?:[1-9][0-9]*)?[0-9]{4})-(1[0-2]|0[1-9])-(3[0-1]|0[1-9]|[1-2][0-9])T(2[0-3]|[0-1][0-9]):([0-5][0-9]):([0-5][0-9])(\.[0-9]+)?(Z|[+-](?:2[0-3]|[0-1][0-9]):[0-5][0-9])?)

		//     NULL : (NULL|[-])
		//       id : shortstring
		//    color : (\(digit,digit,digit\))
		//    value : (([+-])?digit|hex)
		//  boolean : (true|false)
		//   string : quoted string
		// bitfield : (hex|[01]{1-32})
		//     time : iso8601
		//     enum : shortstring

		// builtins :
		//     math : ADD, MULT, DIV, SUB, SIN, COS, LOG, SQR, SQRT
		//   random : RAND, RNDG, RNDN, RRNG, ROLL, PRLN
		// constant : E, PI

		std::vector<std::string> tokens;
		std::vector<pEToken> ptrs;

		bool parsed = false;
		bool haveid = false;
		std::string buf;

		EParser p(defn);	// internal parser

		// we can assume that the input to this function is a minimized definition string.
		for (p.m_context.pos = 1;
			 p.m_context.pos < p.m_contents.length();
			 p.m_context.pos++) {
			char c = p.m_contents[p.m_context.pos];

			if (c == EOF_FIELD_OPEN) {
				p.m_context.flags |= E_PARSE_IN_PAREN;
				buf += c;
			} else if (p.m_context.flags & E_PARSE_IN_PAREN) {
				if (c == EOF_FIELD_CLOSE) {
					p.m_context.flags &= ~E_PARSE_IN_PAREN;
				}

				buf += c;
			} else if ((c == EOF_DOUBLE_QUOTE) ||
					   (c == EOF_SINGLE_QUOTE)) {
				// parse quote
				EString* s = new EString(p.parseQuote());

				ptrs.push_back(s);
				tokens.push_back(s->value());
				parsed = true;
			} else if (c == EOF_DEFN_OPEN) {
				// defn
				p.m_context.pos++;
				p.m_context.cdepth++;

				tokens.push_back(p.parseDefn()->text());
				// TODO: ptrs.push_back(new EToken());
				parsed = true;
			} else if ((c == EOF_TOKEN_DELIM) ||
					   (c == EOF_DEFN_CLOSE)) {
				pEToken tok;

				if (!parsed) {
					// try to interpret the type of buffer, we know it is either
					// a shortstring or some literal value
					if (caseInsensitiveEqual(buf, sTRUE)) {
						// boolean true
						ptrs.push_back(new EBoolean(true));
						tokens.push_back(sTRUE);
					} else if (caseInsensitiveEqual(buf, sFALSE)) {
						// boolean false
						ptrs.push_back(new EBoolean(false));
						tokens.push_back(sFALSE);
					} else if ((buf.length() == 1) && (buf.at(0) == EOF_NULL_CHAR)) {
						// just push a NULL token
						tokens.push_back(buf);
						ptrs.push_back(new EToken());
					} else if ((tok = parseAsIso8601(buf)) != static_cast<pEToken>(0)) {
						ptrs.push_back(tok);
						tokens.push_back(buf);
					} else if ((buf.length() > 3) &&
							   (buf.at(0) == EOF_FIELD_OPEN) &&
							   (buf.at(buf.length() - 1) == EOF_FIELD_CLOSE)) {
						// field
						tokens.push_back(buf);
						ptrs.push_back(new EFieldToken);
					} else {
						pEToken vt;

						// it is a literal, or a group of digits
						// either a float, int, or bitfield value
						if ((vt = isBitvalue(buf)) != static_cast<pEToken>(0)) {
							ptrs.push_back(vt);

							EBitField* bf = dynamic_cast<EBitField*>(vt);
							if (bf) {
								printf("bf := 0x%08x\n", bf->value());
							}
						} else if ((vt = parseValue(buf)) != static_cast<pEToken>(0)) {
							ptrs.push_back(vt);

							EValue* vp = dynamic_cast<EValue*>(vt);
							if (vp) {
								printf("val := %f\n", vp->value().asFloat());
							}
						} else if (verifyShortString(buf)) {
							// id or enum - we will assume that the first shortstring
							// is an id, and all the rest are enum values
							if (!haveid) {
								haveid = true;
								ptrs.push_back(new EId(EShortString(buf)));
							} else {
								ptrs.push_back(new EEnum(EShortString(buf)));
							}
						} else {
							printf("error: unknown or unrecognized type `%s'", buf.c_str());
							ptrs.push_back(new EToken);
						}

						tokens.push_back(buf);
					}
				}

				buf.clear();
				parsed = false;
			} else {
				buf += c;
			}
		}

		for (int i = 0; i < tokens.size(); i++) {
			if (ptrs[i]) {
				printf("token[%02d] := %s (%s)\n", i, tokens[i].c_str(), EToken::TYPE_NAMES[ptrs[i]->type()].c_str());
			} else {
				printf("token[%02d] ignored\n", i, tokens[i].c_str());
			}
		}
	}

	void EParser::parseRoot()
	{
		std::list<pEStatement> statements;
		pEStatement cur;

		for (m_context.pos = 0; m_context.pos < m_contents.length(); m_context.pos++) {
			unsigned char c = m_contents[m_context.pos];
			
			if (c == EOF_DECL_OPEN) {
				m_context.pos++;
				m_context.sdepth++;

				cur = parseDecl();

				if (cur) {
					statements.push_back(cur);
					printf("decl:=%s\n", cur->text().c_str());
				}
			} else if (c == EOF_DEFN_OPEN) {
				m_context.pos++;
				m_context.cdepth++;

				cur = parseDefn();

				if (cur) {
					statements.push_back(cur);
					printf("defn:=%s\n", cur->text().c_str());
					parseTokens(cur->text());
				}
			}
		}

		if (m_context.flags & E_PARSE_IN_ANY)
		{
			// parse error - dangling statement
			printf("syntax error: one or more statements is left open\n");
		}
	}

	pEToken EParser::parseAsIso8601(const std::string& token)
	{
		static const char *formats[] = {
			"%Y-%m-%d",
			"%Y-%m-%d%H:%M:%S",
			"%Y%m%d%H%M%S",
			"%Y-%m-%dT%H:%M:%S",
			"%Y-%m-%d%H:%M:%SZ",
			"%Y%m%d%H%M%SZ",
			"%Y-%m-%dT%H:%M:%SZ",
		};

		static const int sFMT_LEN = sizeof(formats) / sizeof(char*);

		time_t t;
		bool converted = false;
		
		for (int i = 0; i < sFMT_LEN; i++) {
			struct tm tmb;

			const char *buf = token.c_str();
			char *c = strptime(buf, formats[i], &tmb);

			if ((c != NULL) && (*c == '\0')) {
				tmb.tm_isdst = -1;
				t = mktime(&tmb);
				converted = true;

//				printf("iso8601 := %s (%d, %d, %s, %ld)\n", token.c_str(), *c, i, formats[i], t);
				break;
			}
		}

		if (!converted) return static_cast<pEToken>(0);

		return new ETime(t);
	}

	///////////////////////////////////////////////////////////////////////////

	PEValue EParser::func_random_rand()
	{
		PEValue pg;
		pg.fromInt(Rnd::random());

		return pg;
	}

	PEValue EParser::func_random_rndn()
	{
		PEValue pg;
		pg.fromFloat(Rnd::rndn());

		return pg;
	}

	PEValue EParser::func_random_rndg()
	{
		PEValue pg;
		pg.fromFloat(Rnd::rndg());

		return pg;
	}

	PEValue EParser::func_random_range(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			// cue off of the 'a' value to determine integer vs float
			if (av.t == PEValue::INTEGER) {
				pg.fromInt(Rnd::between(av.asInt(), bv.asInt()));
			} else {
				pg.fromFloat(Rnd::betweenf(av.asFloat(), bv.asFloat()));
			}
		}

		return pg;
	}

	PEValue EParser::func_random_roll(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			pg.fromInt(Rnd::ndx(av.asInt(), bv.asInt()));
		}

		return pg;
	}

	PEValue EParser::func_random_perlin(EValue *x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(Rnd::perlin(xv.asFloat()));
		}

		return pg;
	}

	PEValue EParser::func_math_add(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			// cue off of the 'a' value to determine integer vs float
			if (av.t == PEValue::INTEGER) {
				pg.fromInt(av.asInt() + bv.asInt());
			} else {
				pg.fromFloat(av.asFloat() + bv.asFloat());
			}
		}

		return pg;
	}

	PEValue EParser::func_math_mult(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			// cue off of the 'a' value to determine integer vs float
			if (av.t == PEValue::INTEGER) {
				pg.fromInt(av.asInt() * bv.asInt());
			} else {
				pg.fromFloat(av.asFloat() * bv.asFloat());
			}
		}

		return pg;
	}

	PEValue EParser::func_math_div(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			if ((bv.u.i == 0) || (bv.u.f == 0.0f)) return pg;

			// cue off of the 'a' value to determine integer vs float
			if (av.t == PEValue::INTEGER) {
				pg.fromInt(av.asInt() / bv.asInt());
			} else {
				pg.fromFloat(av.asFloat() / bv.asFloat());
			}
		}

		return pg;
	}

	PEValue EParser::func_math_sub(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			// cue off of the 'a' value to determine integer vs float
			if (av.t == PEValue::INTEGER) {
				pg.fromInt(av.asInt() - bv.asInt());
			} else {
				pg.fromFloat(av.asFloat() - bv.asFloat());
			}
		}

		return pg;
	}

	PEValue EParser::func_math_sin(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(sin(xv.asFloat()));
		}

		return pg;
	}

	PEValue EParser::func_math_cos(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(cos(xv.asFloat()));
		}

		return pg;
	}

	PEValue EParser::func_math_log(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(log10(xv.asFloat()));
		}

		return pg;
	}

	PEValue EParser::func_math_sqr(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			if (xv.t == PEValue::INTEGER) {
				int iv = xv.asInt();
				pg.fromInt(iv * iv);
			} else {
				float fv = xv.asFloat();
				pg.fromFloat(fv * fv);
			}
		}

		return pg;
	}

	PEValue EParser::func_math_sqrt(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(sqrt(xv.asFloat()));
		}

		return pg;
	}

	///////////////////////////////////////////////////////////////////////////

	void PETest()
	{
		/////////////////////////
		//
		// chunks can be:
		//    PEValue			float, int, function
		//    std::string		double or single enclosed quotes
		//    bitfield			Defined values separated by '|'
		//	  NULL				- or NULL
		//
		/////////////////////////
		//
		// Colors:
		// Color is constructed from a 4-tuple as outlined below:
		//
		//	func	color	mod		param
		// --
		//	LERP	*		*		v			linear interpolation between color and mod based on param
		//	DARK	*		-		v			darkens color by param
		//	DSAT	*		-		v			desaturates color by param
		//	MULT	*		*		-			multiplies color by mod percent
		//	AUGM	*		-		v			augments color by param
		//	SMTH	*		-		-			smooths color
		//	-		*		-		-			usses color as the color
		//
		//	*  := any color
		//		  this value can be a (r,g,b) integer triplet, or a named color
		//  -  := no input (can also be NULL)
		//  v  := a value, either a functional value (see below), a float, or an int
		//
		// Functions can be used interchangably with values
		//
		/////////////////////////
		//
		// random functions
		//
		// random functions take floats or integers as inputs and return a value,
		// they can be used whenever a specific value is required
		//	
		//	RAND()					returns a random number between 0 and RAND_MAX
		//	RNDN()					returns a random number between 0 and 1
		//	RNDG()					returns a random gaussian distributed number centered at 0
		//	RRNG(a,b)				returns a random number between a and b
		//	ROLL(a,b)				rolls a b-sided dice, returning the sum of their rolls
		//	PRLN(x)					returns 1-d perlin noise around x
		//
		/////////////////////////
		//
		// math functions
		//
		// math functions take values as inputs and return a value,
		// they can be used whenever a specific value is required
		//
		//	ADD(a,b)				returns a + b
		//	MULT(a,b)				returns a * b
		//	DIV(a,b)				returns a / b, b cannot be 0
		//	SUB(a,b)				returns a - b
		//	SIN(x)					returns sine of x
		//	COS(x)					returns cosine of x
		//	LOG(x)					returns base 10 logarithm of x
		//	SQR(x)					returns x * x
		//	SQRT(x)					returns the square root of x
		// contants:
		//	PI						returns 3.14159265359
		//	E						returns 2.71828182846
		//
		// ex: 10+2d6	ADD(10,ROLL(2,6))
		//
		/////////////////////////
		//
		// example (lights.gof)
		//	{ id				range			intensity	func	base				mod		param	}
		//
		//	# comment line
		//  [syntax]		# [syntax]{...} defines the syntax of this file
		//	{
		//		string,		# field type
		//		value,		# field type
		//		value,		# field type
		//		color		# field type
		//  }
		//	# id				range			intensity	color
		//													  func	base				mod		param
		//  { "player_light",	RRNG(14, 18),	1.15,		(255, 255, 171)								  }
		//	[group0.group1.category]
		//  { "magic_tree",		RRNG(12, 16),	1.0,		{ LERP,	sea,				neon,	RNDN()	} }
		//
		// types
		//	 string,			value,			enum,		color,				color,			value


		EParser p("data/test.eof");
		p.parseRoot();
	}

	}
}
