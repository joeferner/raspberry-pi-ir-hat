#ifndef _ARGS_HPP_
#define _ARGS_HPP_

#include <stdbool.h>
#include <stdint.h>

class Args {
private:
    const char* buffer;
    const char* p;

public:
    Args(const char* buffer) {
        this->p = this->buffer = buffer;
    }

    /**
     * matches the beginning of the buffer. If matches consumes those characters
     * and returns true.
     */
    bool read(const char* str);

    bool readComma();

    bool read(uint32_t* value);

    char readChar();

    const char* getWholeBuffer() const;
};

#endif
