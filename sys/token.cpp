#include "token.h"

namespace sys
{

token::token(const token_id id)
{
	setObjectId(id);
}

token::token(const token_type type, unsigned int index)
{
	setObjectId(type, index);
}

token::token()
{
	setObjectId(TOKEN_NONE, 0);
}

token::~token()
{
	// nothing to do
}
	
token& token::operator=(const token &rvalue)
{
	if (this != &rvalue) {
		setObjectId(rvalue.getObjectId());
	}

	return *this;
}

bool token::operator==(const token &rvalue) const
{
	return (getObjectId() == rvalue.getObjectId());
}

bool token::operator==(const token_id &rvalue) const
{
	return (getObjectId() == rvalue);
}

void token::setType(const token_type type)
{
	setObjectId(type, getIndex());
}

void token::setIndex(const unsigned int index)
{
	setObjectId(m_objectType, index);
}

void token::setObjectId(const token_type type, const unsigned int index)
{
	token_id t = static_cast<token_id>(type);
	token_id id = ((t << TOKEN_TYPE_SHIFT) & TOKEN_TYPE_MASK) |
		(index & TOKEN_INDEX_MASK);
	setObjectId(id);
}

void token::setObjectId(const token_id id)
{
	m_objectId = id;
	m_objectType = static_cast<token_type>(
		((id & TOKEN_TYPE_MASK) >> TOKEN_TYPE_SHIFT));

	if (!isValidType(m_objectType)) { m_objectType = TOKEN_NONE; }
}

bool token::isValidType(const token_type type) const
{
	return ((type >= TOKEN_NONE) && (type < TOKEN_LAST));
}

}