/**
 * File        : Error.h
 * Description : Holds a singleton class for error logging.
 */

#include <fstream>
#include <string>

class Log final
{
public:

    /**
     * Output error message to "error.log"
     */
    static void logError (std::string message);

    Log (const Log&)           = delete;
    Log& operator=(const Log&) = delete;

private:
    Log ();

    static Log& get();

    void _logError (std::string message);


    std::ofstream m_errOut;
};
