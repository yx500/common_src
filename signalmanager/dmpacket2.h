#pragma once

//---------------------------------------------------------------------------
#include <stdint.h>
#include <string.h>
#include <algorithm>
#include <ostream>
#include <vector>
#include <ctime>

#include <iostream>
#include <cstdint>
#include <cstring>


// раширение для новых длин до 1024 тоесть 1 килобайта

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma pack(push, 1)

const int TDatagram2_SZ = 1024;
const int TDatagram2_HEADER_LN = 16;
const int TDatagram2_DATA_LN = TDatagram2_SZ - TDatagram2_HEADER_LN;//1008

struct TDatagram2 {
    uint16_t Size;// размер данных в Data
    uint16_t Type;
    char     Name[12];
    uint8_t  Data[TDatagram2_DATA_LN];

    TDatagram2(void) {
        Clear();
    }

/* make trivial assign for lockfreequeue
    TDatagram2(const TDatagram2& c) {
        memcpy(this, &c, sizeof(c));
    }

    TDatagram2& operator = (const TDatagram2& r) {
        memcpy(this, &r, sizeof(r));
        return *this;
    }
*/
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

        void setName(const std::string& name) {
          this->setName( name.c_str() );
        }

    void setData(const void* src, unsigned int sz) {
        Size = sz < sizeof(Data) ? sz : sizeof(Data);
        memcpy(Data, src, Size);
    }

};
#pragma pack(pop)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma pack(push, 4)
struct TDatagramPacket2 {
    uint32_t      tag;     //
    uint32_t      addr;    //адрес от кого
    uint32_t      tick;    //кол пришедших ракетов
    uint32_t      time;    //время пришедшего пакета
    uint32_t      msec;    //время пришедшего пакета msec
    TDatagram2    Dtgrm;

    TDatagramPacket2(void) {
        Clear();
    }

    TDatagramPacket2(const TDatagramPacket2& c) {
        memcpy(this, &c, sizeof(c));
    }

    TDatagramPacket2& operator = (const TDatagramPacket2& r) {
        std::memcpy(this, &r, sizeof(r));
        return *this;
    }

    void Clear(void) {
        std::memset(this, 0, sizeof(TDatagramPacket2));
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
const size_t TDatagramPacket2_HEADER_LN = sizeof(TDatagramPacket2) - sizeof(TDatagram2);


inline std::ostream&  operator<<(std::ostream& os, const TDatagramPacket2& pck2)
{
  std::vector<char> buf(512,0);
  tm* t = localtime( reinterpret_cast<const time_t*>(&pck2.time) );
  snprintf(buf.data(), buf.size(), "%d:%02d\'%02d %02d-%02d-%04d", t->tm_hour, t->tm_min, t->tm_sec, t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
  os<<buf.data();
  os << "\t" << pck2.tick;
  snprintf(buf.data(), buf.size(), "\'%d:%s, sz=%d\'", int(pck2.Dtgrm.Type), pck2.Dtgrm.Name, int(pck2.Dtgrm.Size));
  os<<buf.data();

  for (int i = 0; i < pck2.Dtgrm.Size; ++i) {
    if (i % 16 == 0) os<<std::endl;
    os<<std::hex<<" 0x"<<uint8_t(pck2.Dtgrm.Data[i]);
  }
  return os;
}
