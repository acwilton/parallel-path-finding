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
    void logError (std::string message);

    Log (const Log&) = delete;

    Log& operator=(const Log&) = delete;

    /**
     * Get singleton instance of Log
     */
    static Log& get() const;

private:
    Log ();

    std::ofstream m_errOut;
};
