#include "stdio.h"
#include "stdint.h"
#include "CRC16.h"

#define RINGBUFFSIZE 100

typedef struct {
    uint8_t head;
    uint8_t tail;
    uint8_t length;
    uint8_t buff[RINGBUFFSIZE];
}ringBuffer_t;
ringBuffer_t ringBuff = {
    0,
    0,
    0,
    {0},
};
int isBuffFull(void);
int WriteBuff(uint8_t);
int readBuff(uint8_t*);

int main()
{
    printf("ringBuffer Test.\n");
    
    return 1;
}



int isBuffFull()
{
    return ringBuff.length >= RINGBUFFSIZE;
}

int isBuffEmpty()
{
    return ringBuff.length == 0;
}

int WriteBuff(uint8_t data)
{
    if(isBuffFull())
        return -1;
    
    ringBuff.buff[ringBuff.tail] = data;
    ringBuff.tail = (ringBuff.tail + 1) % RINGBUFFSIZE;
    ringBuff.length++;
    return 1;
}

int readBuff(uint8_t *pdata)
{
    if(isBuffEmpty())
        return -1;
    
    *pdata = ringBuff.buff[ringBuff.head];
    ringBuff.head = (ringBuff.head + 1) % RINGBUFFSIZE;
    ringBuff.length--;
    return 1;
}