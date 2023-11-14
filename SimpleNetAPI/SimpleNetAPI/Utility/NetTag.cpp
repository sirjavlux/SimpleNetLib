#include "NetTag.h"

NetTag::NetTag(const char InCharArray[])
{
	SetCharArray(InCharArray, sizeof(InCharArray));
}

NetTag::NetTag(const NetTag& InTag)
{
	*this = InTag;
}

NetTag::NetTag()
{
	SetCharArray(DEFAULT_NET_TAG, sizeof(DEFAULT_NET_TAG));
}

NetTag::~NetTag()
{
	delete[] charArray_;
}

void NetTag::SetCharArray(const char* InCharArray, const uint64_t InSize)
{
	size_ = InSize;
	
	delete[] charArray_;
	charArray_ = new char[size_];

	std::memmove(charArray_, InCharArray, size_ * sizeof(char));

	HashName();
}

void NetTag::operator=(const std::string& InString)
{
	SetCharArray(InString.c_str(), InString.length());
}

void NetTag::operator=(const NetTag& InTag)
{
	SetCharArray(InTag.charArray_, InTag.size_);
}

void NetTag::HashName()
{
	static std::hash<char> hasher;
	
}
