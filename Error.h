/**
 * File        : Error.h
 * Description : Holds a singleton class for error logging.
 */

#ifndef ERROR_H
#define ERROR_H

#include <fstream>
#include <string>

class Log final
{
public:

    /**
     * Output error message to "gui.log"
     */
    static void logError (std::string message);

    /**
     * Output warning message to "gui.log"
     */
    static void logWarning (std::string message);

    /**
     * Output info message to "gui.log"
     */
    static void logInfo (std::string message);

    Log (const Log&) = delete;
    Log& operator= (const Log&) = delete;

private:
    Log ();

    static Log& get ();

    void _logError (std::string message);
    void _logWarning (std::string message);
    void _logInfo (std::string message);

    std::ofstream m_errOut;
};

#endif /* ERROR_H */
