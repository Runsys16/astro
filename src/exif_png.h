#ifndef EXIF_PNG_H
#define EXIF_PNG_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <png.h>
//------------------------------------------------------------------------------------------------------
using namespace std;
//------------------------------------------------------------------------------------------------------
// Structures binaires strictement alignées sur 1 octet (Little-Endian)
#pragma pack(push, 1)
struct TiffHeader {
    uint16_t byteOrder = 0x4949; // "II" (Little-Endian)
    uint16_t magic     = 0x002A; // 42
    uint32_t ifd0Offset = 8;     // L'IFD0 suit immédiatement l'en-tête
};

struct IfdEntry {
    uint16_t tag;
    uint16_t type;
    uint32_t count;
    uint32_t valueOrOffset;
};
#pragma pack(pop)

// Types TIFF standards
enum class TiffType : uint16_t {
    BYTE      = 1,
    ASCII     = 2,
    SHORT     = 3,
    LONG      = 4,
    RATIONAL  = 5
};
//------------------------------------------------------------------------------------------------------
class ExifPngBuilder {
private:
	struct TagItem {
		uint16_t		tag;
		uint16_t		type;
		uint32_t		count;
		vector<uint8_t>	rawData;
	};

	vector<TagItem> ifd0Entries;

public:
	void 				addAscii(uint16_t , const string&);
	void 				addShort(uint16_t , uint16_t );
	void 				addRational(uint16_t , uint32_t , uint32_t );
	vector<uint8_t>		build();


};
//------------------------------------------------------------------------------------------------------

#endif
