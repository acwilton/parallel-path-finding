/**
 * Error.cc
 */

#include "Error.h"

const std::string ERROR_PREFIX = "(EE) - ";

void Log::logError (std::string message)
{
    get()._logError(message);
}

Log::Log ()
: m_errOut ("error.log")
{
    m_errOut << "Error logging initialized." << std::endl;
}

Log& Log::get ()
{
    static Log l;
    return l;
}

void Log::_logError (std::string message)
{
    m_errOut << ERROR_PREFIX << message << std::endl;
}


