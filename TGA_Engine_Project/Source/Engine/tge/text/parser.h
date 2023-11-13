#ifndef _PARSER_H_
#define _PARSER_H_

#include "Token.h"

#define NO_TOKEN	0

//===============================================================
class Parser
{
//---------------------------------------------------------------
// Members
private:

protected:
	std::vector<Token*>	m_pToken; // this array holds the CTokens, this Parser parses for

public:
//---------------------------------------------------------------
// Methods
private:

protected:

public:
	virtual		~Parser();
	bool		Add(int tokenID, std::string tokenAsStr);
	int			CheckForToken(unsigned char currentChar);
	void		Reset();
};

#endif // _PARSER_H_
