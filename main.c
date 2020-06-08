#include "stdio.h"
#include "stdint.h"
#include "CRC16.h"
#include "stdbool.h"

#define RINGBUFFSIZE 50

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
void testRingBuff(void);

int main()
{
    printf("ringBuffer Test.\n\n");
    testRingBuff();
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
        return false;
    
    ringBuff.buff[ringBuff.tail] = data;
    ringBuff.tail = (ringBuff.tail + 1) % RINGBUFFSIZE;
    ringBuff.length++;
    return true;
}

int readBuff(uint8_t *pdata)
{
    if(isBuffEmpty())
        return false;
    
    *pdata = ringBuff.buff[ringBuff.head];
    ringBuff.head = (ringBuff.head + 1) % RINGBUFFSIZE;
    ringBuff.length--;
    return true;
}

void parseBDprotocol()//解析半岛的通讯协议
{
    //从缓冲区读数据，
    uint8_t pdata[100];
    uint8_t i = 0;
    uint8_t len = 0;
    // readBuff(&pdata[i]);
    while(readBuff(&pdata[i])){
        if(pdata[i] == '$'){
            i++;
            readBuff(&pdata[i]);
            if(pdata[i] == 'B'){
                i++;
                readBuff(&pdata[i]);
                if(pdata[i] == 'D'){
                    i++;
                    //确认是半岛协议开始字头，开始获取整条指令的长度
                    readBuff(&pdata[i++]);
                    readBuff(&pdata[i++]);
                    readBuff(&pdata[i]);//指令长度字节
                    len = pdata[i];
                    // printf("len = %d\n", len);
                    i++;
                    for(;i<len;i++){
                        readBuff(&pdata[i]);
                    }
                    //执行for循环后，i=12
                    //将完整的指令转存到了数组pdata中，进行协议解析，CRC16校验
                    uint16_t crc = CRC16(pdata, len-3);
                    uint8_t c_h8 = crc >> 8;
                    uint8_t c_l8 = crc & 0xff;
                    for(int j=0;j<len;j++){
                        printf("pdata[%02d] = 0x%02x \n", j, pdata[j]);
                    }
                    printf("H8 = 0x%02x, L8 = 0x%02x \n", c_h8, c_l8);
                }
            }
        }
        i = 0;
    }

}

void testRingBuff()
{
    //                  sync           des   pkg   len   ~len  cmd   para  crc   crc   end
    //                  b0   b1   b2   b3    b4    b5    b6    b7    b8    b9    b10   b11
    uint8_t data1[26] = {0x0f, '$', 'B', 'D', 0xea, 0x01, 0x0c, 0xf3, 0x01, 0xab, 0xb8, 0x5a, 0x04, 0x0f,
                         '$', 'B', 'D', 0xea, 0x01, 0x0c, 0xf3, 0x02, 0xab, 0xd6, 0xde, 0x04};
    //将数据data1拷贝进ringbuff
    uint8_t len = sizeof(data1);
    // printf("len = %d", len);
    for(int i=0;i<len;i++){
        WriteBuff(data1[i]);
    }
    parseBDprotocol();
}