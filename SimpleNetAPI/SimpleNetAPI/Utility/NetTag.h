#pragma once
#include "../NetIncludes.h"

#define DEFAULT_NET_TAG "None"

class NET_LIB NetTag
{
public:
	explicit NetTag(const char InCharArray[]);
	NetTag(const NetTag& InTag);
	NetTag();
	~NetTag();

protected:
	uint64_t hash_;

	uint64_t size_;
	char* charArray_ = nullptr;

private:
	void SetCharArray(const char* InCharArray, const uint64_t InSize);

	void operator=(const std::string& InString);

	void operator=(const NetTag& InTag);

	void HashName();
};