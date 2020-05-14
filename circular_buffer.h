//
// Created by Maksim Piriyev on 5/15/20.
//

#ifndef TESTZOON_CIRCULAR_BUFFER_H
#define TESTZOON_CIRCULAR_BUFFER_H


#include <stdint.h>
#include <string.h>


class circular_buffer {
#define MAX_BUFFER_SIZE (10*1024)
    uint8_t buffer[MAX_BUFFER_SIZE];
    unsigned int i = 0;
public:
    unsigned int length = 0;

    uint8_t operator[](int j) { return buffer[(j + i) % MAX_BUFFER_SIZE]; }

    void operator++() { length++; }

    void operator--() {
        i = (i + 1) % MAX_BUFFER_SIZE;
        length--;
    }

    void operator+=(uint8_t u) {
        //#define min(a, b) ((a)<(b)?(a):(b));
        //#define max(a, b) ((a)>(b)?(a):(b))
        buffer[(i + length) % MAX_BUFFER_SIZE] = u;
//            length = min(length+1,MAX_BUFFER_SIZE);
        length = length+1 < MAX_BUFFER_SIZE ? length+1 : MAX_BUFFER_SIZE;
    }
//    void operator+=(std::string s) {
//        for(int i=0;i<s.size();i++){
//            (*this) += (uint8_t) s[i];
//        }
//    }
    void operator+=(const char* s) {
        while(*s != '\0'){
            (*this) += (uint8_t) *s;
            s++;
        }
    }

    operator uint8_t() { return buffer[i]; }

    void reset() {
        i = 0;
        length = 0;
    }

    circular_buffer() { reset();}

    circular_buffer(circular_buffer &b) {
        for (int j = 0; j < b.length; j++) {
            buffer[j] = b[j];
        }
        length = b.length;
    }

    void copy(const uint8_t *data, int size) {
        for (int j = 0; j < size; j++) {
            (*this) += data[j];
        }
    }

    void skip(int j) {
        j = j < length ? j : length;
        i = (i + j) % MAX_BUFFER_SIZE;
        length -= j;
    }
    //MavlinkType mavlinkType(int i);
};

#endif //TESTZOON_CIRCULAR_BUFFER_H
