/**
 * Error.cc
 */

#include "Error.h"

Log::Log ()
: m_errOut ("error.log")
{
    m_errOut << "Error logging initialized." << std::endl;
}

static


