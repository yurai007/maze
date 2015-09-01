#ifndef BYTE_BUFFER_HPP
#define BYTE_BUFFER_HPP

#include <cstring>
#include <array>
#include <cassert>
#include <vector>

namespace serialization
{

const static int max_size = 256;

struct byte_buffer
{
	byte_buffer() : offset(0) {}

	void put_char(char value)
	{
		memcpy(&m_byte_buffer[offset], &value, sizeof(char));
		offset += sizeof(char);
	}

	void put_bool(bool value)
	{
		memcpy(&m_byte_buffer[offset], &value, sizeof(bool));
		offset += sizeof(bool);
	}

	void put_int(int value)
	{
		memcpy(&m_byte_buffer[offset], &value, sizeof(int));
		offset += sizeof(int);
	}

	void put_long(long value)
	{
		memcpy(&m_byte_buffer[offset], &value, sizeof(long));
		offset += sizeof(long);
	}

	void put_double_vector(const std::vector<double> &value)
	{
		size_t size = value.size()*sizeof(double);
		put_int(size);
		memcpy(&m_byte_buffer[offset], &value[0], size);
		offset += size;
	}

	void put_long_vector(const std::vector<long> &value)
	{
		size_t size = value.size()*sizeof(long);
		put_int(size);
		memcpy(&m_byte_buffer[offset], &value[0], size);
		offset += size;
	}

	void put_string(const std::string &value)
	{
		size_t size = value.size()*sizeof(char);
		put_int(size);
		memcpy(&m_byte_buffer[offset], &value[0], size);
		offset += size;
	}

	char get_char()
	{
		char value;
		memcpy(&value, &m_byte_buffer[offset], sizeof(char));
		offset += sizeof(char);
		return value;
	}

	bool get_bool()
	{
		bool value;
		memcpy(&value, &m_byte_buffer[offset], sizeof(bool));
		offset += sizeof(bool);
		return value;
	}

	int get_int()
	{
		int value;
		memcpy(&value, &m_byte_buffer[offset], sizeof(int));
		offset += sizeof(int);
		return value;
	}

	long get_long()
	{
		long value;
		memcpy(&value, &m_byte_buffer[offset], sizeof(long));
		offset += sizeof(long);
		return value;
	}

	void get_double_vector(std::vector<double> &value)
	{
		size_t size = get_int();
		assert(size <= value.size());
		memcpy(&value[0], &m_byte_buffer[offset], size);
		offset += size;
	}

	void get_long_vector(std::vector<long> &value)
	{
		size_t size = get_int();
		assert(size <= value.size());
		memcpy(&value[0], &m_byte_buffer[offset], size);
		offset += size;
	}

	// TO DO: move semantics to avoid coping?
	std::string get_string()
	{
		size_t size = get_int();
		std::string result(&m_byte_buffer[offset], &m_byte_buffer[offset + size]);
		offset += size;
		return result;
	}

	void clear()
	{
		offset = 0;
	}

	int get_size()
	{
		return offset;
	}

	std::array<unsigned char, max_size> m_byte_buffer;
	int offset;
};

}

#endif // BYTE_BUFFER_HPP
