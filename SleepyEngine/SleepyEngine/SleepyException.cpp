#include "SleepyException.h"
#include <sstream>

SleepyException::SleepyException( int line,const char* file ) noexcept
	:
	line( line ),
	file( file )
{
}

const char* SleepyException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* SleepyException::GetType() const noexcept
{
	return "Sleepy Exception";
}

int SleepyException::GetLine() const noexcept
{
	return line;
}

const std::string& SleepyException::GetFile() const noexcept
{
	return file;
}

std::string SleepyException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}
