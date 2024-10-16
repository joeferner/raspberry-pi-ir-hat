#include "Args.hpp"
#include <stdlib.h>

bool Args::read(const char* str) {
    const char* newP = this->p;
    while (*str) {
        if (*str != *newP) {
            return false;
        }
        str++;
        newP++;
    }
    this->p = newP;
    return true;
}

bool Args::readComma() {
    if (*this->p == ',') {
        this->p++;
        return true;
    }
    return false;
}

bool Args::read(uint32_t* value) {
    char temp[10];
    char* pTemp = temp;
    char* pTempEnd = temp + sizeof(temp);
    const char* newP = p;

    while (*newP >= '0' && *newP <= '9' && pTemp != pTempEnd) {
        *pTemp++ = *newP++;
    }
    *pTemp = '\0';

    if (pTemp > temp) {
        *value = atoi(temp);
        this->p = newP;
        return true;
    }

    return false;
}

char Args::readChar() {
    char ch = *this->p;
    if (*this->p != '\0') {
        this->p++;
    }
    return ch;
}

const char* Args::getWholeBuffer() const {
    return buffer;
}
