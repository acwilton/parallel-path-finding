/**
 * Error.cc
 */

#include "gui/Error.h"

const std::string ERROR_PREFIX = "(EE)";
const std::string WARNING_PREFIX = "(WW)";
const std::string INFO_PREFIX = "(II)";
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

void Log::logInfo (std::string message)
{
    get ()._logInfo (message);
}

Log::Log ()
        : m_errOut (LOG_FILE_NAME)
{
    m_errOut << "Logging initialized." << std::endl << std::flush;
}

Log& Log::get ()
{
    static Log l;
    return l;
}

void Log::_logError (std::string message)
{
    m_errOut << ERROR_PREFIX << LOG_SEPARATOR << message << std::endl << std::flush;
}

void Log::_logWarning (std::string message)
{
    m_errOut << WARNING_PREFIX << LOG_SEPARATOR << message << std::endl << std::flush;
}

void Log::_logInfo (std::string message)
{
    m_errOut << INFO_PREFIX << LOG_SEPARATOR << message << std::endl << std::flush;
}

