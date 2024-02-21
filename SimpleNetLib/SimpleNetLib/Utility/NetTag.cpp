#include "NetTag.h"

const char* NetTag::ToCStr() const
{
	return data_;
}

std::string NetTag::ToStr() const
{
	return std::string(data_, size_);
}

bool NetTag::IsValid() const
{
	return *this == INVALID_NET_TAG;
}

NetTag::NetTag(const char InCharArray[])
{
	SetCharArray(InCharArray, strlen(InCharArray));
}

NetTag::NetTag(const NetTag& InTag)
{
	*this = InTag;
}

NetTag::NetTag() : NetTag(INVALID_NET_TAG)
{
	
}

NetTag::~NetTag()
{
	delete[] data_;
}

uint64_t NetTag::GetHash() const
{
	return hash_;
}

void NetTag::SetCharArray(const char* InCharArray, const uint64_t InSize)
{
	size_ = InSize;
	
	char* newData = new (std::nothrow) char[size_ + 1];
	if (newData == nullptr)
	{
		return;
	}
	
	std::memmove(newData, InCharArray, size_);
	newData[size_] = '\0';
	
	delete[] data_;
	
	data_ = newData;
	
	HashName();
}

void NetTag::HashName()
{
	hash_ = 0;
	for (uint64_t i = 0; i < size_; ++i)
	{
		hash_ = (hash_ << 5) ^ (hash_ >> 27) ^ static_cast<uint64_t>(data_[i]);
	}
}

NetTag& NetTag::operator=(const std::string& InString)
{
	SetCharArray(InString.c_str(), InString.size());
	return *this;
}

NetTag& NetTag::operator=(const char InCharArray[])
{
	SetCharArray(InCharArray, strlen(InCharArray));
	return *this;
}

NetTag& NetTag::operator=(const NetTag& InTag)
{
	if (this != &InTag)
	{
		SetCharArray(InTag.data_, InTag.size_);
	}
	return *this;
}

bool NetTag::operator==(const NetTag& InTag) const
{
	return hash_ == InTag.hash_;
}

bool NetTag::operator==(const uint64_t& InHash) const
{
	return hash_ == InHash;
}

bool NetTag::operator!=(const NetTag& InTag) const
{
	return hash_ != InTag.hash_;
}

bool NetTag::operator!=(const uint64_t& InHash) const
{
	return hash_ != InHash;
}

bool NetTag::operator<(const NetTag& InTag) const
{
	return hash_ < InTag.hash_;
}

bool NetTag::operator<(const uint64_t& InHash) const
{
	return hash_ < InHash;
}
