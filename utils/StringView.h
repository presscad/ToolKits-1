#pragma once

#include "string.h"
#include<iostream>
#include "assert.h"

class string_view {
public:
	string_view()
		: _ptr(nullptr),
		_length(0)
	{
	}

	string_view(const char* str)
		: _ptr(str),
		_length(strlen(_ptr))
	{
	}

	string_view(const unsigned char* str)
		: _ptr(reinterpret_cast<const char*>(str)),
		_length(strlen(_ptr))
	{
	}

	string_view(const std::string& str)
		: _ptr(str.data()),
		_length(static_cast<size_t>(str.length()))
	{
	}

	string_view(const char* offset, size_t len)
		: _ptr(offset),
		_length(len)
	{
	}

	const char* data() const { return _ptr; }

	size_t size() const { return _length; }

	bool empty() const { return _length == 0; }

	const char* begin() const { return _ptr; }

	const char* end() const { return _ptr + _length; }

	void clear()
	{
		_ptr = nullptr; 
		_length = 0; 
	}

	void set(const char* buffer, size_t len)
	{
		_ptr = buffer;
		_length = len;
	}

	void set(const char* str)
	{
		_ptr = str;
		_length = strlen(str);
	}

	void set(const void* buffer, size_t len)
	{
		_ptr = reinterpret_cast<const char*>(buffer);
		_length = len;
	}

	char operator[](size_t i) const { return _ptr[i]; }

	void remove_prefix(size_t n)
	{
		assert(n <= _length);
		_ptr += n;
		_length -= n;
	}

	void remove_suffix(size_t n)
	{
		assert(n <= _length);
		_length -= n;
	}

	bool operator==(const string_view& x) const
	{
		return ((_length == x._length) &&
			(memcmp(_ptr, x._ptr, _length) == 0));
	}

	bool operator!=(const string_view& sp) const
	{
		return !(*this == sp);
	}

	std::string asString() const 
	{ 
		return std::string(data(), size());
	} 

	void copyToStdString(std::string* target) const
	{
		assert(target != nullptr);
		target->assign(_ptr, _length);
	}

	bool startWith(const string_view& x) const
	{
		return ((_length >= x._length) && 
			(memcmp(_ptr, x._ptr, x._length) == 0));
	}

	int compare(const string_view& x) const 
	{
		int r = memcmp(_ptr, x._ptr, _length < x._length ? _length : x._length);
		if (r == 0)
		{
			if (_length < x._length) r = -1;
			else if (_length > x._length) r = +1;
		}
		return r;
	}
private:
	const char* _ptr;
	size_t _length;
};
