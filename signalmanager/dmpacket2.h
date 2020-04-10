#ifndef DmPacket2H
#define DmPacket2H
//---------------------------------------------------------------------------
#include <string.h>
#include <algorithm>
#include <stdint.h>

//до 1024 тоесть 1 килобайта
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define __time32_t quint32

#pragma pack(push, 1)

const size_t TDatagram2_SZ = 1024;
const size_t TDatagram2_HEADER_LN = 16;
const size_t TDatagram2_DATA_LN = TDatagram2_SZ - TDatagram2_HEADER_LN;//1008

struct TDatagram2
{
    uint16_t Size;// размер данных в Data
    uint16_t Type;
    char      Name[12];
    uint8_t  Data[TDatagram2_DATA_LN];

    TDatagram2(void) {
        Clear();
    }

    TDatagram2(const TDatagram2& c) {
        memcpy(this, &c, sizeof(c));
    }

    TDatagram2& operator = (const TDatagram2& r) {
        memcpy(this, &r, sizeof(r));
        return *this;
    }

    void Clear(void) {
        memset(this, 0, sizeof(*this));
    }

    bool operator == (const TDatagram2& r) const {
        return !memcmp(this, &r, sizeof(r));
    }
    bool operator != (const TDatagram2& r) const {
        return memcmp(this, &r, sizeof(r));
    }

    void setName(const char* name) {
        strncpy(Name, name, sizeof(Name));
        Name[11] = 0;
    }

    void setData(const void* src, size_t sz) {
        memcpy(Data, src, std::min(sizeof(Data), sz));
    }

};
#pragma pack(pop)



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma pack(push, 4)
struct TDatagramPacket2
{
    uint32_t     tag;     //
    uint32_t     addr;    //адрес от кого
    uint32_t     tick;    //кол пришедших ракетов
    __time32_t        time;       //время пришедшего пакета
    uint32_t     msec;    //время пришедшего пакета msec
    TDatagram2    Dtgrm;

    TDatagramPacket2(void) {
        Clear();
    }

    TDatagramPacket2(const TDatagramPacket2& c) {
        memcpy(this, &c, sizeof(c));
    }

    TDatagramPacket2& operator = (const TDatagramPacket2& r) {
        memcpy(this, &r, sizeof(r));
        return *this;
    }

    void Clear(void) {
        memset(this, 0, sizeof(*this));
    }

    bool IsDataEqual(const TDatagramPacket2& r) const {
        return !memcmp(&this->Dtgrm.Data[0], &r.Dtgrm.Data[0], sizeof(r.Dtgrm.Data));
    }

    bool operator == (const TDatagramPacket2& r) const {
        return !memcmp(this, &r, sizeof(r));
    }
    bool operator != (const TDatagramPacket2& r) const {
        return memcmp(this, &r, sizeof(r));
    }
};
#pragma pack(pop)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef TDatagramPacket2 TPackEx2;
const int TDatagramPacket2_HEADER_LN = sizeof(TDatagramPacket2) - sizeof(TDatagram2);





//---------------------------------------------------------------------------
#endif
