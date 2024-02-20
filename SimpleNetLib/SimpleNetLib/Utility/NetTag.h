#pragma once
#include "../NetIncludes.h"

#define INVALID_NET_TAG NetTag("None")

class NET_LIB_EXPORT NetTag
{
public:
	const char* ToCStr() const;
	std::string ToStr() const;

	bool IsValid() const;
	
	explicit NetTag(const char InCharArray[]);
	NetTag(const NetTag& InTag);
	NetTag();
	~NetTag();

	uint64_t GetHash() const;
	
	NetTag& operator=(const std::string& InString);
	NetTag& operator=(const char InCharArray[]);
	NetTag& operator=(const NetTag& InTag);
	
	bool operator==(const NetTag& InTag) const;
	bool operator==(const uint64_t& InHash) const;
	bool operator!=(const NetTag& InTag) const;
	bool operator!=(const uint64_t& InHash) const;
	
	bool operator<(const NetTag& InTag) const;
	bool operator<(const uint64_t& InHash) const;
	
protected:
	uint64_t hash_;
	uint64_t size_;
	char* data_ = nullptr;

private:
	void SetCharArray(const char* InCharArray, const uint64_t InSize);

	void HashName();
};