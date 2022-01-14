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
    bool read(const char* str) {
        const char* newP = this->p;
        while (*str) {
            if (*str != *newP) {
                return false;
            }
            newP++;
        }
        this->p = newP;
        return true;
    }

    bool readComma() {
        if (*this->p == ',') {
            this->p++;
            return true;
        }
        return false;
    }

    bool read(uint32_t* value) {
        char temp[10];
        char* pTemp = temp;
        char* pTempEnd = temp + sizeof(temp);
        const char* newP = p;

        while (*newP >= '0' && *newP <= '9' && pTemp != pTempEnd) {
            *pTemp++ = *newP++;
        }

        if (pTemp > temp) {
            *value = atoi(temp);
            return true;
        }

        return false;
    }

    char readChar() {
        char ch = *this->p;
        if (*this->p != '\0') {
            this->p++;
        }
        return ch;
    }

    const char* getWholeBuffer() const {
        return buffer;
    }
};

#endif
