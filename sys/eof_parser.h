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

#define EOF_NULL_CHAR		'-'
#define EOF_END_OF_STR		'\0'


#define EOF_DEFN_DEPTH		16		// max number of nested definitions
#define EOF_FUNC_DEPTH		32		// max number of nested functions

#define EOF_VERSION			"0.7"

//							256 Mb
#define EOF_PARSER_FILEMAX	268435456

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
			E_FIELD,
			E_DEFN,
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

		Type type() const { return m_type;  }

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
	// EOF ShortString														 //
	///////////////////////////////////////////////////////////////////////////

	class EShortString
	{
	public:
		EShortString();
		EShortString(const std::string& sz);
		EShortString(const char* sz);

		virtual ~EShortString();

		// returns this string as a const char*
		const char* str() const;

		// returns the length of this short string
		unsigned int length() const;

		EShortString& operator=(const EShortString& rhs);

		// calls compare()
		bool operator==(const EShortString& rhs) const;

		// lexicographical less than
		bool operator<(const EShortString &rhs) const;

		// compares this short string to the given string based on the
		// given case sensitivity.  true is returned if they are equal
		bool compare(const std::string& sz, bool caseSensitive = true) const;
		bool compare(const char* sz, bool caseSensitive = true) const;
		bool compare(const EShortString& sz, bool caseSensitive = true) const;
		bool compare(char c, bool caseSensitive = true) const;

		// assigns this short string to the given value
		void assign(const std::string& sz);
		void assign(const char* sz);
		void assign(const EShortString& sz);
		void assign(char c, unsigned int n = 1);	// fill

		// clear, empty
		void clear();
		bool empty() const;

		// adds a short string or a char to the end of this short string
		// if able
		EShortString& operator+=(const EShortString& rhs);
		EShortString& operator+=(char rhs);

		// bracket operator
		char& operator[](int n);
		const char& operator[](int n) const;

		// at
		char at(int n) const;

		static bool isshort(unsigned char c);

	protected:

		unsigned int m_length;
		static const unsigned int sSHORT_STRING_LEN = 127;

		char m_string[sSHORT_STRING_LEN + 1];
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

		_PEValue operator=(const _PEValue& rhs)
		{
			if (this != &rhs) {
				t = rhs.t;
				u = rhs.u;
			}

			return *this;
		}
	};

	typedef _PEValue PEValue;

	// Value base class
	class EValue : public EToken
	{
	public:
		EValue() : EToken(E_VALUE) {}
		virtual ~EValue() {}

		virtual EValue* copy() = 0;
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

		virtual EValue* copy();
		virtual PEValue value();

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

		virtual EValue* copy();
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

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function base class

	class EValueFunc : public EValue
	{
	public:
		explicit EValueFunc();
		virtual ~EValueFunc();

		virtual PEValue value() = 0;
		virtual PEValue value(EValue* v1, EValue* v2) = 0;
		virtual void args(EValue* v1 = static_cast<EValue*>(0), EValue* v2 = static_cast<EValue*>(0));

		virtual EValue* copy() = 0;
		virtual int nArgs() const = 0;

	protected:

		EValue* m_param1;
		EValue* m_param2;
	};

	inline
	void EValueFunc::args(EValue* v1, EValue* v2)
	{
		m_param1 = v1;
		m_param2 = v2;
	}

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking no Value arguments

	class EValueFuncNoArgs : public EValueFunc
	{
	public:
		EValueFuncNoArgs(pEValueFuncNoArgs func);
		virtual ~EValueFuncNoArgs() {}

		virtual PEValue value();
		virtual PEValue value(EValue* v1, EValue* v2);

		virtual EValue* copy();
		virtual int nArgs() const;

	protected:

		pEValueFuncNoArgs m_func;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking one Value argument

	class EValueFuncOneArg : public EValueFunc
	{
	public:
		EValueFuncOneArg(pEValueFuncOneArg func);
		EValueFuncOneArg(pEValueFuncOneArg func, EValue* param);
		virtual ~EValueFuncOneArg() {}

		virtual PEValue value();
		virtual PEValue value(EValue* v1, EValue* v2);

		virtual EValue* copy();
		virtual int nArgs() const;

	protected:

		pEValueFuncOneArg m_func;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Value function taking two Value arguments

	class EValueFuncTwoArgs : public EValueFunc
	{
	public:
		EValueFuncTwoArgs(pEValueFuncTwoArgs func);
		EValueFuncTwoArgs(pEValueFuncTwoArgs func, EValue* p1, EValue* p2);
		virtual ~EValueFuncTwoArgs() {}

		virtual PEValue value();
		virtual PEValue value(EValue* v1, EValue* v2);

		virtual EValue* copy();
		virtual int nArgs() const;

	protected:

		pEValueFuncTwoArgs m_func;
	};

	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// EOF Field															 //
	///////////////////////////////////////////////////////////////////////////

	class EField
	{
	public:

		unsigned int size() const;

		// clear, empty
		void clear();
		bool empty() const;
		
		// randomize field values
		void shuffle();

		// sums field values
		PEValue sum() const;

		// performs func on each value
		void each(EValueFuncOneArg* func);
		void each(pEValueFuncOneArg func);
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Generator Token Properties										 //
	///////////////////////////////////////////////////////////////////////////

	class EGeneratorToken : public EToken
	{
	public:
		EGeneratorToken() : EToken(E_DEFN) {}
		virtual ~EGeneratorToken() {}
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Color Properties													 //
	///////////////////////////////////////////////////////////////////////////

	class EColor : public EGeneratorToken
	{
	public:
		EColor() : EGeneratorToken() {}
		virtual ~EColor() {}

		virtual Color value() = 0;
	};

	class EColorConst : public EColor
	{
	public:
		// {value,value,value}|enum
		virtual Color value();

	protected:

		Color m_color;
	};

	class EColorDynamic : public EColor
	{
	public:
		// {function,color,color,value}
		// function: LERP|DARK|DSAT|MULT|AUGM|SMTH|GLIT|GLUM|GAVE
		// color: EColorConst|EColorDynamic
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
	// EOF Boolean Properties												 //
	///////////////////////////////////////////////////////////////////////////

	class EBoolean : public EToken
	{
	public:
		EBoolean(bool b = false);
		virtual ~EBoolean();

		virtual bool value();

	protected:
		bool m_bool;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Time Properties													 //
	///////////////////////////////////////////////////////////////////////////

	class ETime : public EToken
	{
	public:
		ETime();
		ETime(time_t t);
		virtual ~ETime();

		virtual time_t value();
	protected:

		time_t m_time;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Bitfield Properties												 //
	///////////////////////////////////////////////////////////////////////////

	class EBitField : public EToken
	{
	public:
		EBitField();
		EBitField(unsigned int field);
		virtual ~EBitField();

		virtual unsigned int value();

		void set(unsigned int bit);
		bool isSet(unsigned int bit) const;

		static const unsigned int BIT_MAX;
	protected:

		unsigned int m_field;
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF Field Token Properties											 //
	///////////////////////////////////////////////////////////////////////////

	class EFieldToken : public EToken
	{
	public:
		EFieldToken() : EToken(E_FIELD) {}

		virtual ~EFieldToken() {}

		void addField(const PEValue& v) {}
	};

	///////////////////////////////////////////////////////////////////////////
	// EOF ShortToken Properties											 //
	///////////////////////////////////////////////////////////////////////////

	class EShortToken : public EToken
	{
	public:
		EShortToken(const EShortString& sz, const Type& type) : EToken(type), m_string(sz) {}
		virtual ~EShortToken() {}

		EShortString value();

	protected:

		EShortString m_string;
	};

	inline
	EShortString EShortToken::value()
	{
		return m_string;
	}
	

	class EEnum : public EShortToken
	{
	public:
		EEnum(const EShortString& sz) : EShortToken(sz, E_ENUM) {}
		virtual ~EEnum() {}
	};

	class EId : public EShortToken
	{
	public:
		EId(const EShortString& sz) : EShortToken(sz, E_ID) {}
		virtual ~EId() {}
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
		EParser(const std::string& sz);
		EParser(const char* filename);
		~EParser();

		// This operation parses statements at the root level.  Statements may
		// contain other nested statements - this operation does not parse those
		// it returns full string statements from the root level (including bracket
		// delimiters).
		void parseRoot();

		static bool ishex(unsigned char c);
		static bool isbinary(unsigned char c);

	protected:

		pEStatement parseDecl();
		pEStatement parseDefn();

		std::string parseQuote(bool keepslashes = false);
		pEToken parseField();

		void parseTokens(const std::string& defn);
		EValue* parseLiteral(const std::string& val) const;
		pEToken parseValue(const std::string& val);

		bool verifyShortString(const std::string& sz) const;
		bool caseInsensitiveEqual(const std::string& sz, const std::string& buf) const;
		pEToken isBitvalue(const std::string& val) const;

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

		struct EBuiltinTable
		{
			std::string name;
			pEToken		token;
		};

		static const EBuiltinTable FuncTable[];
		static const EBuiltinTable ConstTable[];

		void resetContext();

	protected:

		FILE *m_fp;

		std::string m_contents;
		EParserContext m_context;

	private:

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
	};

	// tests

	void PETest();

	}
}
