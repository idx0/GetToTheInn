#pragma once

namespace sys
{

typedef unsigned int token_id;

enum token_type
{
	TOKEN_FIRST = 0,

	TOKEN_NONE = TOKEN_FIRST,
	TOKEN_LISTENER,
	TOKEN_EVENT,
	TOKEN_RENDERER,
	TOKEN_WIDGET,
	TOKEN_DATA,

	TOKEN_LAST,
	TOKEN_COUNT = (TOKEN_LAST - TOKEN_FIRST)
};

#define TOKEN_INDEX_MASK	0x003fffff
#define TOKEN_TYPE_MASK	0xffc00000
#define TOKEN_TYPE_SHIFT	22

class token
{
public:
	explicit token(const token_id id);
	explicit token(const token_type type, unsigned int index);
	explicit token();

	virtual ~token();

	token& operator=(const token &rvalue);

	bool operator==(const token &rvalue) const;
	bool operator==(const token_id &rvalue) const;

	token_type getType() const { return m_objectType; }
	unsigned int getIndex() const;
	token_id getObjectId() const { return m_objectId; }

	void setType(const token_type type);
	void setIndex(const unsigned int index);
	void setObjectId(const token_id id);
	void setObjectId(const token_type type, const unsigned int index);

protected:
	bool isValidType(const token_type type) const;

private:
	token_id m_objectId;
	token_type m_objectType;
};

inline
unsigned int token::getIndex() const
{
	return (m_objectId & TOKEN_INDEX_MASK);
}

}
