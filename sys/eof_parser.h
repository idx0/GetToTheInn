#pragma once

#include <string>

#include "../common.h"

#define EOF_DEFN_OPEN		'{'
#define EOF_DEFN_CLOSE		'}'
#define EOF_DECL_OPEN		'['
#define EOF_DECL_CLOSE		']'
#define EOF_FIELD_OPEN		'('
#define EOF_FIELD_CLOSE		')'

#define EOF_TOKEN_DELIM		','
#define EOF_DECL_DELIM		':'

#define EOF_DOUBLE_QUOTE	'"'
#define EOF_SINGLE_QUOTE	'\''

#define EOF_ESCAPE_CHAR		'\\'
#define EOF_COMMENT_CHAR	'#'

namespace sys {
	namespace eof {
	// Evolve Object Format

	///////////////////////////////////////////////////////////////////////////
	// EOF Base Definitions													 //
	///////////////////////////////////////////////////////////////////////////

	class EToken
	{
	public:
		enum Type {
			E_NULL = 0,
			E_ID,
			E_VALUE,
			E_COLOR,
			E_STRING,
			E_BITFIELD,
			E_TIME,
			E_BOOL,
			E_ENUM,

			NUM_TOKENS
		};

	public:
		EToken() : m_type(E_NULL) {}
		EToken(const Type& type) : m_type(type) {}

		virtual ~EToken() {}

		static const std::string TYPE_NAMES[];

	protected:

		Type m_type;
	};

	typedef EToken* pEToken;

	class EStatement
	{
	public:
		EStatement() {}
		virtual ~EStatement() {}

		// returns the text string for this statement
		virtual std::string text() const = 0;
	};

	typedef EStatement* pEStatement;

	class EDefinition : public EStatement
	{
	public:
		EDefinition(const std::string& sz);
		virtual ~EDefinition();

		virtual std::string text() const;

	protected:

		std::string m_string;
	};

	class EDeclaration : public EStatement
	{
	public:
		virtual std::string text() const = 0;
	};


	///////////////////////////////////////////////////////////////////////////
	// EOF Value Properties													 //
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// A structure defining a value as a POD type float or int

	struct _PEValue
	{
		enum { INTEGER = 0, FLOAT } t;

		union {
			int i;
			float f;
		} u;

		_PEValue() : t(INTEGER) { u.i = 0; }

		int asInt() const;
		float asFloat() const;
		void fromInt(int i);
		void fromFloat(float f);
	};

	typedef _PEValue PEValue;

	// Value base class
	class EValue : public EToken
	{
	public:
		EValue() : EToken(E_VALUE) {}
		virtual ~EValue() {}

		virtual PEValue value() = 0;
	};

	///////////////////////////////////////////////////////////////////////////
	// Float value class - This class represents a constant floating point
	// value

	class EValueFloat : public EValue
	{
	public:
		EValueFloat(float f);
		virtual ~EValueFloat();

		void set(float f);
		virtual PEValue value();

		static const EValueFloat PI;
		static const EValueFloat E;

	protected:

		float m_value;
	};

	inline
	void EValueFloat::set(float f)
	{
		m_value = f;
	}

	///////////////////////////////////////////////////////////////////////////
	// Integer value class - This class represents a constant integer value

	class EValueInt : public EValue
	{
	public:
		EValueInt(int i);
		virtual ~EValueInt();

		void set(int i);
		virtual PEValue value();

	protected:

		int m_value;
	};

	inline
	void EValueInt::set(int i)
	{
		m_value = i;
	}

	///////////////////////////////////////////////////////////////////////////
	// EOF Value Function Definitions

	typedef PEValue (*pEValueFuncNoArgs)(void);
	typedef PEValue (*pEValueFuncOneArg)(EValue*);
	typedef PEValue (*pEValueFuncTwoArgs)(EValue*, EValue*);

	struct FunctionTable
	{
		std::string			name;
		pEValueFuncNoArgs	func0;
		pEValueFuncOneArg	func1;
		pEValueFuncTwoArgs	func2;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function base class

	class EValueFunc : public EValue
	{
	public:
		explicit EValueFunc();
		virtual ~EValueFunc() {}

		virtual PEValue value() = 0;

		static const FunctionTable FuncTable[];

	protected:

		EValue* m_param1;
		EValue* m_param2;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking no Value arguments

	class EValueFuncNoArgs : public EValueFunc
	{
	public:
		EValueFuncNoArgs(pEValueFuncNoArgs func);
		virtual ~EValueFuncNoArgs() {}

		virtual PEValue value();

	protected:

		pEValueFuncNoArgs m_func;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking one Value argument

	class EValueFuncOneArg : public EValueFunc
	{
	public:
		EValueFuncOneArg(pEValueFuncOneArg func, EValue* param);
		virtual ~EValueFuncOneArg() {}

		virtual PEValue value();

	protected:

		pEValueFuncOneArg m_func;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking two Value arguments

	class EValueFuncTwoArgs : public EValueFunc
	{
	public:
		EValueFuncTwoArgs(pEValueFuncTwoArgs func, EValue* p1, EValue* p2);
		virtual ~EValueFuncTwoArgs() {}

		virtual PEValue value();

	protected:

		pEValueFuncTwoArgs m_func;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value Function Declairations

	// Random
	static PEValue func_random_rand();
	static PEValue func_random_rndn();
	static PEValue func_random_rndg();
	static PEValue func_random_range(EValue* a, EValue *b);
	static PEValue func_random_roll(EValue* a, EValue *b);
	static PEValue func_random_perlin(EValue *x);

	// Math
	static PEValue func_math_add(EValue* a, EValue *b);
	static PEValue func_math_mult(EValue* a, EValue *b);
	static PEValue func_math_div(EValue* a, EValue *b);
	static PEValue func_math_sub(EValue* a, EValue *b);
	static PEValue func_math_sin(EValue* x);
	static PEValue func_math_cos(EValue* x);
	static PEValue func_math_log(EValue* x);
	static PEValue func_math_sqr(EValue* x);
	static PEValue func_math_sqrt(EValue* x);

	///////////////////////////////////////////////////////////////////////////
	// EOF Color Properties													 //
	///////////////////////////////////////////////////////////////////////////

	class EColor : public EToken
	{
	public:
		EColor() : EToken(E_COLOR) {}
		virtual ~EColor() {}

		virtual Color value() = 0;
	};

	class EColorConst : public EColor
	{

		virtual Color value();

	protected:

		Color m_color;
	};

	class EColorDynamic : public EColor
	{
	public:
		enum Function
		{
			E_COLOR_NONE = 0,
			E_COLOR_LERP,
			E_COLOR_DARK,
			E_COLOR_DSAT,
			E_COLOR_MULT,
			E_COLOR_AUGM,
			E_COLOR_SMTH,

			E_COLOR_GLIT,	// greyscale lightness
			E_COLOR_GLUM,	// greyscale luminosity
			E_COLOR_GAVE,	// greyscale average

		};

	public:

		virtual Color value();

	protected:

		Function m_func;
		Color m_base;
		Color m_mod;
		EValue* m_value;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF String Properties												 //
	///////////////////////////////////////////////////////////////////////////

	class EString : public EToken
	{
	public:
		EString();
		EString(const std::string& sz);
		virtual ~EString();

		virtual std::string value();

	protected:

		std::string m_string;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Declarations Classes												 //
	///////////////////////////////////////////////////////////////////////////

	class EDeclNull : public EDeclaration
	{
	public:
		EDeclNull();
		virtual ~EDeclNull();

		virtual std::string text() const;
	};

	class EDeclKeyValue : public EDeclaration
	{
	public:
		EDeclKeyValue(const std::string& k, const std::string& v);
		virtual ~EDeclKeyValue();

		std::string key() const;
		std::string value() const;

		virtual std::string text() const;

	protected:
		std::string m_key;
		std::string m_value;
	};

	class EDeclSection : public EDeclaration
	{
	public:
		EDeclSection(const std::string& p);
		virtual ~EDeclSection();

		std::string path() const;

		virtual std::string text() const;

	protected:

		std::string m_path;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Parsing Classes													 //
	///////////////////////////////////////////////////////////////////////////

	class EObject
	{
		friend class EObjectBuilder;
	public:


	protected:
		typedef std::vector<EToken::Type> Syntax;
		typedef std::vector<EToken*> Tokens;

		Syntax m_syntax;
		Tokens m_tokens;
	};

	class EObjectBuilder
	{
	public:
		// parses the syntax definition given by defn printing an error
		// and returning false on failure.  On success, true is returned
		// and the current syntax is set to the parsed values.
		bool loadSyntax(const std::string& defn);

	protected:
		EObject::Syntax m_currentSyntax;
		std::vector<EObject*> m_objects;
	};

	class EParser
	{
	public:
		EParser(const char* filename);
		~EParser();

		// This operation parses statements at the root level.  Statements may
		// contain other nested statements - this operation does not parse those
		// it returns full string statements from the root level (including bracket
		// delimiters).
		void parseRoot();

		static bool isshort(unsigned char c);

	protected:

		pEStatement parseDecl();
		pEStatement parseDefn();

		std::string parseQuote();
		void parseField();

		void parseToken();

		bool verifyShortString(const std::string& sz);

		static unsigned char specialEscape(unsigned char c);

		typedef pEToken (*ParseAsFunc)(const std::string&);

		// ParseAsFunc for time - uses iso8601
		static pEToken parseAsIso8601(const std::string& token);

		// returns the digit in token as val, returning true if the token
		// could be parsed as a digit
//		bool parseDigit(const std::string& token, PEValue* val);


	protected:

		enum EParserFlags
		{
			E_PARSE_ROOT		= 0,
			E_PARSE_IN_SQUOTE	= B1(0),
			E_PARSE_IN_DQUOTE	= B1(1),
			E_PARSE_IN_CURLY	= B1(2),
			E_PARSE_IN_SQUARE	= B1(3),
			E_PARSE_IN_PAREN	= B1(4),

			E_PARSE_COMMENT		= B1(5),

			E_PARSE_IN_STRING	= (E_PARSE_IN_SQUOTE | E_PARSE_IN_DQUOTE),
			E_PARSE_IN_ANY		= (E_PARSE_IN_STRING | E_PARSE_IN_CURLY |
								   E_PARSE_IN_SQUARE | E_PARSE_IN_PAREN)
		};

		struct EParserContext
		{
			unsigned int pos;
			unsigned int start;

			unsigned int flags;

			int pdepth;	// paren depth
			int cdepth;	// curly depth
			int sdepth;	// square depth
		};

		void resetContext();

	protected:

		FILE *m_fp;

		std::string m_contents;
		EParserContext m_context;
	};

	// tests

	void PETest();

	}
}
