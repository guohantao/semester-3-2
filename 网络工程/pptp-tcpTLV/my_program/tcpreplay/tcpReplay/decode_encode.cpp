#include "server.h"
#include <QDebug>

quint16 rawDecodeUit16(const char *data , int pos)
{
    //大端转小端
    //低地址-高位：将data低地址数值左移8位，即放入Result的高地址中
    quint16 result = (quint8)data[pos] * 256 + (quint8)data[pos+1];
    return result;
}

quint32 rawDecodeUit32(const char *data , int pos)
{
     //同理
    quint32 result = (quint8)data[pos]*256*256*256 + (quint8)data[pos+1]*256*256 + (quint8)data[pos+2]*256 + (quint8)data[pos+3];
    return result;
}

void rawEncodeUnit16(char *data , int pos , quint16 value)
{
    //小端转大端
    //将Value高位右移8位，放入data低地址中
    data[pos+0] = (value >> 8) & 0x00ff;
    data[pos+1] = (value >> 0) & 0x00ff;
}

void rawEncodeUnit32(char *data, int pos, quint32 value)
{
    data[pos+0] = (value >> 24) & 0x00ff;
    data[pos+1] = (value >> 16) & 0x00ff;
    data[pos+2] = (value >> 8) & 0x00ff;
    data[pos+3] = (value >> 0) & 0x00ff;
}
