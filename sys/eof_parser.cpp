#include "eof_parser.h"
#include "thread_os.h"
#include "../rnd.h"

#include <stdlib.h>
#include <sstream>
#include <list>

namespace sys {
	namespace eof {

	int _PEValue::asInt() const
	{
		if (t == INTEGER) return u.i;
		return 0;
	}

	float _PEValue::asFloat() const
	{
		if (t == FLOAT) return u.f;
		return 0.0f;
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

	const EValueFloat EValueFloat::PI(3.14159265359f);
	const EValueFloat EValueFloat::E(2.71828182846f);

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

	///////////////////////////////////////////////////////////////////////////

	EValueFunc::EValueFunc() :
		m_param1(NULL), m_param2(NULL)
	{
		// nothing
	}

	EValueFuncNoArgs::EValueFuncNoArgs(pEValueFuncNoArgs func) : m_func(func)
	{
		// nothing
	}

	EValueFuncOneArg::EValueFuncOneArg(pEValueFuncOneArg func, EValue* param) : m_func(func)
	{
		m_param1 = param;
	}

	EValueFuncTwoArgs::EValueFuncTwoArgs(pEValueFuncTwoArgs func, EValue* p1, EValue* p2) : m_func(func)
	{
		m_param1 = p1;
		m_param2 = p2;
	}

	PEValue EValueFuncNoArgs::value()
	{
		return m_func();
	}

	PEValue EValueFuncOneArg::value()
	{
		return m_func(m_param1);
	}

	PEValue EValueFuncTwoArgs::value()
	{
		return m_func(m_param1, m_param2);
	}

	///////////////////////////////////////////////////////////////////////////

	const FunctionTable EValueFunc::FuncTable[] = {
		{ "RAND",	func_random_rand,	NULL,				NULL				},
		{ "RNDN",	func_random_rndn,	NULL,				NULL				},
		{ "RNDG",	func_random_rndg,	NULL,				NULL				},
		{ "RRNG",	NULL,				NULL,				func_random_range	},
		{ "ROLL",	NULL,				NULL,				func_random_roll	},
		{ "PRLN",	NULL,				func_random_perlin,	NULL				},
		{ "ADD",	NULL,				NULL,				func_math_add		},
		{ "MULT",	NULL,				NULL,				func_math_mult		},
		{ "DIV",	NULL,				NULL,				func_math_div		},
		{ "SUB",	NULL,				NULL,				func_math_sub		},
		{ "SIN",	NULL,				func_math_sin,		NULL				},
		{ "COS",	NULL,				func_math_cos,		NULL				},
		{ "SQR",	NULL,				func_math_sqr,		NULL				},
		{ "SQRT",	NULL,				func_math_sqrt,		NULL				},
	};
	// function not found.  functions first argument will be used as value.

	///////////////////////////////////////////////////////////////////////////

	PEValue func_random_rand()
	{
		PEValue pg;
		pg.fromInt(Rnd::random());

		return pg;
	}

	PEValue func_random_rndn()
	{
		PEValue pg;
		pg.fromFloat(Rnd::rndn());

		return pg;
	}

	PEValue func_random_rndg()
	{
		PEValue pg;
		pg.fromFloat(Rnd::rndg());

		return pg;
	}

	PEValue func_random_range(EValue* a, EValue *b)
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

	PEValue func_random_roll(EValue* a, EValue *b)
	{
		PEValue pg;

		if ((a) && (b)) {
			PEValue av = a->value();
			PEValue bv = b->value();

			pg.fromInt(Rnd::ndx(av.asInt(), bv.asInt()));
		}

		return pg;
	}

	PEValue func_random_perlin(EValue *x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(Rnd::perlin(xv.asFloat()));
		}

		return pg;
	}

	PEValue func_math_add(EValue* a, EValue *b)
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

	PEValue func_math_mult(EValue* a, EValue *b)
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

	PEValue func_math_div(EValue* a, EValue *b)
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

	PEValue func_math_sub(EValue* a, EValue *b)
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

	PEValue func_math_sin(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(std::sin(xv.asFloat()));
		}

		return pg;
	}

	PEValue func_math_cos(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(std::cos(xv.asFloat()));
		}

		return pg;
	}

	PEValue func_math_sqr(EValue* x)
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

	PEValue func_math_sqrt(EValue* x)
	{
		PEValue pg;

		if (x) {
			PEValue xv = x->value();

			pg.fromFloat(std::sqrt(xv.asFloat()));
		}

		return pg;
	}

	///////////////////////////////////////////////////////////////////////////

	EString::EString()
	{
		// nothing
	}

	EString::EString(const std::string& sz) : m_string(sz)
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

	EParser::EParser(const char* filename)
	{
#ifdef __EVOLVE_WIN32__
		fopen_s(&m_fp, filename, "r");
#else
		m_fp = fopen(filename, "r");
#endif

		resetContext();
		char *buffer;

		if (m_fp) {
			std::fseek(m_fp, 0, SEEK_END);
			size_t cap = std::ftell(m_fp);
			std::rewind(m_fp);

			buffer = new char[cap];
			fread(buffer, 1, cap, m_fp);
			m_contents.assign(buffer);
			delete[] buffer;
		}
	}

	EParser::~EParser()
	{
		fclose(m_fp);
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

	bool EParser::isshort(unsigned char c)
	{
		return (((c >= 'A') && (c <= 'Z')) ||
				((c >= 'a') && (c <= 'z')) ||
				((c >= '0') && (c <= '9')) ||
				(c == '_'));
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
					switch (c) {
					default: break;
					case 'n': c = '\n'; break;
					case 'r': c = '\r'; break;
					case 't': c = '\t'; break;
					}

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
				sstr.append(parseQuote());
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
	
	std::string EParser::parseQuote()
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
					statement += c;
					escaped = false;
				} else {
					escaped = true;
				}
			} else if (escaped) {
				// we just saw an escaped character, is it something special?
				switch (c) {
				default: statement += c; break;
				case 'n': statement += '\n'; break;
				case 'r': statement += '\r'; break;
				case 't': statement += '\t'; break;
				}

				escaped = false;
			} else if ((m_context.flags & E_PARSE_IN_DQUOTE) && (c == EOF_DOUBLE_QUOTE)) {
				m_context.flags &= ~E_PARSE_IN_DQUOTE;
				break;
			} else if ((m_context.flags & E_PARSE_IN_SQUOTE) && (c == EOF_SINGLE_QUOTE)) {
				m_context.flags &= ~E_PARSE_IN_SQUOTE;
				break;
			} else {
				statement += c;
			}
		}

//		printf("quote:=%s\n", statement.c_str());
		return statement;
	}

	void EParser::parseField()
	{
	}

	void EParser::parseToken()
	{
		unsigned char start = m_contents[m_context.pos];

		// try to determine the type of token we are trying to parse


		for (; m_context.pos < m_contents.length(); m_context.pos++) {
			unsigned char c = m_contents[m_context.pos];

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
				}
			}
		}

		if (m_context.flags & E_PARSE_IN_ANY)
		{
			// parse error - dangling statement
			printf("syntax error: one or more statements is left open\n");
		}
	}

	///////////////////////////////////////////////////////////////////////////

	void PETest()
	{
		EValueFuncNoArgs v1(func_random_rndg);
		EValueFloat v2(EValueFloat::PI);

		EValueFuncTwoArgs rb(func_math_mult, &v1, &v2);

		printf("func_math_mult: %3.3f\n", rb.value().asFloat());


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


		EParser p("data/lights.eof");
		p.parseRoot();
	}

	}
}