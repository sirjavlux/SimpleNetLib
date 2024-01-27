#include "NetTag.h"

const char* NetTag::ToCStr() const
{
	return static_cast<char*>(data_);
}

std::string NetTag::ToStr() const
{
	return std::string { ToCStr(), static_cast<unsigned long>(size_) };
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

void NetTag::SetCharArray(const char* InCharArray, const uint64_t InSize)
{
	size_ = InSize;

	// Allocate + 1 for null termination
	delete[] data_;
	data_ = new char[size_ + 1];
	
	std::memmove(data_, InCharArray, size_);
	data_[size_] = '\0';
	
	HashName();
}

void NetTag::HashName()
{
	for (uint64_t i = 0; i < size_; i += 1)
	{
		hash_ = (hash_ ^ (std::hash<char>()(data_[i]) << 1)) >> 1;
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
		// Ensure destination object has enough memory
		if (size_ != InTag.size_)
		{
			delete[] data_;
			size_ = InTag.size_;
			data_ = new char[size_ + 1];
		}

		// Copy data
		std::memmove(data_, InTag.data_, size_);
		data_[size_] = '\0';

		hash_ = InTag.hash_;
	}

	return *this;
}

bool NetTag::operator==(const NetTag& InTag) const
{
	return hash_ == InTag.hash_;
}

bool NetTag::operator!=(const NetTag& InTag) const
{
	return hash_ != InTag.hash_;
}
