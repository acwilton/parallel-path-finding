/**
 * Error.cc
 */

#include "Error.h"

const std::string ERROR_PREFIX = "(EE)";
const std::string WARNING_PREFIX = "(WW)";
const std::string LOG_SEPARATOR = " - ";
const std::string LOG_FILE_NAME = "gui.log";

void Log::logError (std::string message)
{
    get ()._logError (message);
}

void Log::logWarning (std::string message)
{
    get ()._logWarning (message);
}

Log::Log ()
        : m_errOut (LOG_FILE_NAME)
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
    m_errOut << ERROR_PREFIX << LOG_SEPARATOR << message << std::endl;
}

void Log::_logWarning (std::string message)
{
    m_errOut << WARNING_PREFIX << LOG_SEPARATOR << message << std::endl;
}

