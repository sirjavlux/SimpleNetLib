#include "stdafx.h"
#include <tge/text/Parser.h>
#include <sstream>

#define MAX_ERR_MESSAGE_LENGTH	1024

///////////////////////////////////////////////////////////////////////////////////////////
Parser::~Parser()
{
	int i = static_cast<int>(m_pToken.size()) - 1;
	while (i >= 0)	// toggle through the list of pointers
	{
		Token* pCurrentToken = m_pToken[i];
		delete pCurrentToken;	// remove the element the current pointer points at and...
		i--;
	}
	m_pToken.clear();  // ...remove all pointers
}
///////////////////////////////////////////////////////////////////////////////////////////
bool Parser::Add(int tokenID, std::string tokenAsStr)
{
	if (tokenAsStr == "" || tokenID == NO_TOKEN)
	{
		return false;
	}
	//-------------------------------------------------------------------------------------
	/*std::string tokenAsstd(tokenAsStr);
	int i = m_pToken.size()-1;
	while (i >= 0)	// toggle through the list of pointers
	{
		if (tokenID == m_pToken[i]->m_tokenID)
		{
			std::stringstream errMessageStr;
			errMessageStr << "Parser::Add(...) error: tokenID " << tokenID << " already occupied." << std::endl;
			TRACE(errMessageStr);
			return false;
		}
		if (tokenAsstd::string.Find(m_pToken[i]->m_tokenAsStr)!=-1 || m_pToken[i]->m_tokenAsStr.Find(tokenAsstd::string)!=-1)
		{
			std::string errMessageStr;
			errMessageStr.Format("Parser::Add(...) error: tokenAsStr \"%s\" matches an existing token- substring or vice versa.\n", tokenAsStr);
			TRACE(errMessageStr);
			return false;
		}
		i--;
	}*/
	//-------------------------------------------------------------------------------------
	Token* pToken = new Token(tokenID, tokenAsStr);
	
	if (pToken == NULL)
	{
		return false;
	}

	m_pToken.push_back(pToken);

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////
// When reading the file character by character, this method checks if a token has been found.
// The tokens recognized by traceGURU are defined by the array m_token.
// For each character that matches a token, the corresponding token-counter is increased.
// If the current character does not match, the counter is reset to 0.
// As soon as a token-counter reaches the length of the corresponding token-string, a token
// was found.
int Parser::CheckForToken(unsigned char currentChar)
{
	int i = static_cast<int>(m_pToken.size());
	while (i--) // foreach token
	{
		Token dummyToken = *m_pToken[i];
		std::string dummy = m_pToken[i]->m_tokenAsStr;

		if (currentChar == m_pToken[i]->m_tokenAsStr[m_pToken[i]->m_matchPos])
		{ // if the currentChar matches the requested character of the current token,...
			m_pToken[i]->m_matchPos++; // ...increase the "match-position" counter
			if (m_pToken[i]->m_matchPos == (int)m_pToken[i]->m_tokenAsStr.size()) 
			{ // if the counter equals the length of the current token, we found a token. Thus,...
				//...reset the counters of all the other tokens and...
				this->Reset();

				// ...return the token found
				return m_pToken[i]->m_tokenID;
			}
		}
		else // if the currentChar does *not* match the requested character of the current token,...
		{
			m_pToken[i]->m_matchPos = 0; // reset the corresponding counter
		}
	}
	return NO_TOKEN;
}
///////////////////////////////////////////////////////////////////////////////////////////
void Parser::Reset()
{
	unsigned int i = static_cast<int>(m_pToken.size());//-1 + 1;
	while (i--)
	{
		m_pToken[i]->m_matchPos = 0;
	}
}
