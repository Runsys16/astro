//-------------------------------------------------------------------------------------------------------------------
//
// A partir du code source IA gemini de google
//
//-------------------------------------------------------------------------------------------------------------------
#define EXIF_PNG_CPP
//-------------------------------------------------------------------------------------------------------------------
#include "exif_png.h"
//-------------------------------------------------------------------------------------------------------------------
//
// Ajouter une chaîne ASCII (le '\0' final est géré automatiquement)
//
//-------------------------------------------------------------------------------------------------------------------
void ExifPngBuilder::addAscii(uint16_t tag, const string& str) {
    vector<uint8_t> data(str.begin(), str.end());
    data.push_back('\0');
    ifd0Entries.push_back({tag, static_cast<uint16_t>(TiffType::ASCII), static_cast<uint32_t>(data.size()), data});
}
//-------------------------------------------------------------------------------------------------------------------
//
// Ajouter un entier 16 bits (SHORT)
//
//-------------------------------------------------------------------------------------------------------------------
void ExifPngBuilder::addShort(uint16_t tag, uint16_t value) {
    vector<uint8_t> data(2);
    memcpy(data.data(), &value, 2);
    ifd0Entries.push_back({tag, static_cast<uint16_t>(TiffType::SHORT), 1, data});
}
//-------------------------------------------------------------------------------------------------------------------
//
// Ajouter un rationnel (2 x uint32_t : numérateur / dénominateur)
//
//-------------------------------------------------------------------------------------------------------------------
void ExifPngBuilder::addRational(uint16_t tag, uint32_t numerator, uint32_t denominator) {
    vector<uint8_t> data(8);
    memcpy(data.data(), &numerator, 4);
    memcpy(data.data() + 4, &denominator, 4);
    ifd0Entries.push_back({tag, static_cast<uint16_t>(TiffType::RATIONAL), 1, data});
}
//-------------------------------------------------------------------------------------------------------------------
//
// Générer le tampon binaire brut prêt pour le chunk PNG "eXIf"
//
//-------------------------------------------------------------------------------------------------------------------
vector<uint8_t> ExifPngBuilder::build() {
    // 1. Trier les balises par ordre croissant de Tag ID (exigence TIFF)
    sort(ifd0Entries.begin(), ifd0Entries.end(), [](const TagItem& a, const TagItem& b) {
        return a.tag < b.tag;
    });

    // 2. Calculer les tailles et offsets
    uint16_t entryCount = static_cast<uint16_t>(ifd0Entries.size());
    uint32_t ifdSize = 2 + (entryCount * sizeof(IfdEntry)) + 4; // Count + Entries + NextIFD(0)
    uint32_t currentDataOffset = sizeof(TiffHeader) + ifdSize;

    vector<IfdEntry> compiledEntries;
    vector<uint8_t> overflowData;

    for (const auto& item : ifd0Entries) {
        IfdEntry entry{};
        entry.tag = item.tag;
        entry.type = item.type;
        entry.count = item.count;

        if (item.rawData.size() <= 4) {
            // La valeur tient directement dans les 4 octets
            entry.valueOrOffset = 0;
            memcpy(&entry.valueOrOffset, item.rawData.data(), item.rawData.size());
        } else {
            // La valeur est stockée après l'IFD, on inscrit son offset
            entry.valueOrOffset = currentDataOffset;
            overflowData.insert(overflowData.end(), item.rawData.begin(), item.rawData.end());
            currentDataOffset += static_cast<uint32_t>(item.rawData.size());
        }
        compiledEntries.push_back(entry);
    }

    // 3. Assembler le buffer final
    vector<uint8_t> buffer;
    buffer.reserve(currentDataOffset);

    // En-tête TIFF (8 octets)
    TiffHeader header;
    const uint8_t* hdrPtr = reinterpret_cast<const uint8_t*>(&header);
    buffer.insert(buffer.end(), hdrPtr, hdrPtr + sizeof(TiffHeader));

    // Nombre d'entrées (2 octets)
    const uint8_t* cntPtr = reinterpret_cast<const uint8_t*>(&entryCount);
    buffer.insert(buffer.end(), cntPtr, cntPtr + 2);

    // Table des balises (12 octets * N)
    const uint8_t* entPtr = reinterpret_cast<const uint8_t*>(compiledEntries.data());
    buffer.insert(buffer.end(), entPtr, entPtr + compiledEntries.size() * sizeof(IfdEntry));

    // Offset IFD suivant = 0 (4 octets)
    uint32_t nextIfd = 0;
    const uint8_t* nextPtr = reinterpret_cast<const uint8_t*>(&nextIfd);
    buffer.insert(buffer.end(), nextPtr, nextPtr + 4);

    // Données déportées (> 4 octets)
    buffer.insert(buffer.end(), overflowData.begin(), overflowData.end());

    return buffer;
}

