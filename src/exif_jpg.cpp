//-------------------------------------------------------------------------------------------------------------------
//
// A partir du code source IA gemini de google
// --------------------------------------------
// Entierement  reconstruit 
// start() recupe les donnees exif deja presente
// ... ajout des donnees exis avec les fontions :set_..._tag()
// stop() melange les nouvelles donnees avec les anciennes
//
// get_out_buff() recupere le buffer de donnees exis+imùage
// en vue de la sauvegarde
//
//-------------------------------------------------------------------------------------------------------------------
#define EXIF_JPG_CPP
//-------------------------------------------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------------------------------------------
#include "exif_jpg.h"
//-------------------------------------------------------------------------------------------------------------------
//
// Creation et mise en place de la structure exif
//
//-------------------------------------------------------------------------------------------------------------------
bool ExifJpgBuilder::start( const vector<unsigned char>& in_jpeg )
{
    srcinfo.err = jpeg_std_error( &jsrcerr );

    jpeg_create_decompress( &srcinfo );
    jpeg_mem_src( &srcinfo, in_jpeg.data(), static_cast<unsigned long>(in_jpeg.size()) );

    // Sauvegarder TOUS les marqueurs (y compris APP1 pour lire l'EXIF et conserver le XMP)
    jpeg_save_markers( &srcinfo, JPEG_COM, 0xFFFF );
    for (int m = 0; m < 16; ++m) {
        jpeg_save_markers( &srcinfo, JPEG_APP0 + m, 0xFFFF );
    }

    jpeg_read_header(&srcinfo, TRUE);
    jvirt_barray_ptr* coef_arrays = jpeg_read_coefficients(&srcinfo);

    // Repérer l'éventuel marqueur APP1 contenant "Exif\0\0"
    //const unsigned char* old_exif_data = nullptr;
    //unsigned int old_exif_len = 0;
    
    for ( jpeg_saved_marker_ptr marker = srcinfo.marker_list; marker != nullptr; marker  = marker->next ) {
        if (marker->marker == (JPEG_APP0 + 1) &&
            marker->data_length > 6 &&
            memcmp(marker->data, "Exif\0\0", 6) == 0) {
            old_exif_data = marker->data;
            old_exif_len = marker->data_length;
            break;
        }
    }
	//--------------------------------------------------------------------------------
	
    dstinfo.err = jpeg_std_error(&jdsterr);
    jpeg_create_compress(&dstinfo);

    jpeg_mem_dest(&dstinfo, &out_buf, &out_size);

    jpeg_copy_critical_parameters(&srcinfo, &dstinfo);
    jpeg_write_coefficients(&dstinfo, coef_arrays);
    
    // Vérifier si un bloc EXIF valide ("Exif\0\0") existait dans l'image source
    if (old_exif_data && old_exif_len > 6 && memcmp(old_exif_data, "Exif\0\0", 6) == 0) {
        exif = exif_data_new_from_data(old_exif_data, old_exif_len);
    }

    // Si aucun EXIF n'existait, partir d'une structure vierge
    if (!exif) {
        exif = exif_data_new();
        if (!exif) return false;
        exif_data_set_option( 		exif, EXIF_DATA_OPTION_FOLLOW_SPECIFICATION);
        exif_data_set_data_type( 	exif, EXIF_DATA_TYPE_COMPRESSED);
        exif_data_set_byte_order(	exif, EXIF_BYTE_ORDER_INTEL);
        exif_data_fix(exif);
    }

    return true;
}
//-------------------------------------------------------------------------------------------------------------------
//
// Sauvegarde des infos exif
// Build le buffer de sortie
//
//-------------------------------------------------------------------------------------------------------------------
bool ExifJpgBuilder::stop()
{
    exif_data_save_data(exif, &new_exif_data, &new_exif_len);
    exif_data_unref(exif);
    
    if ( (new_exif_data != nullptr && new_exif_len > 0) )
    {
        if (new_exif_len <= 65533) {
            jpeg_write_marker(&dstinfo, JPEG_APP0 + 1, new_exif_data, new_exif_len);
        }
        free(new_exif_data);
    }

    // 2. Recopier tous les autres marqueurs (ICC, commentaires, et même le XMP qui utilise aussi APP1 !)
    for (jpeg_saved_marker_ptr marker = srcinfo.marker_list; marker != nullptr; marker = marker->next) {
        bool is_old_exif = (marker->marker == (JPEG_APP0 + 1) &&
                            marker->data_length > 6 &&
                            memcmp(marker->data, "Exif\0\0", 6) == 0);
        if (!is_old_exif) {
            jpeg_write_marker(&dstinfo, marker->marker, marker->data, marker->data_length);
        }
    }

    jpeg_finish_compress(&dstinfo);
    jpeg_destroy_compress(&dstinfo);
    jpeg_finish_decompress(&srcinfo);
    jpeg_destroy_decompress(&srcinfo);

    if (out_buf && out_size > 0) {
        out_jpeg.assign(out_buf, out_buf + out_size);
        free(out_buf);
        return true;
    }
    return false;
    

}
//-------------------------------------------------------------------------------------------------------------------
//
// Récupère un tag existant dans l'IFD ou le crée s'il n'existe pas encore
//
//-------------------------------------------------------------------------------------------------------------------
ExifEntry* ExifJpgBuilder::get_or_create_entry( ExifIfd ifd, ExifTag tag)
{
    ExifEntry* entry = exif_content_get_entry( exif->ifd[ifd], tag );
    if (!entry) {
        entry = exif_entry_new();
        entry->tag = tag;
        exif_content_add_entry(exif->ifd[ifd], entry);
        exif_entry_unref(entry); // L'IFD détient la référence
    }
    return entry;
}
//-------------------------------------------------------------------------------------------------------------------
//
// 1. Ajouter ou remplacer un tag ASCII
//
//-------------------------------------------------------------------------------------------------------------------
void ExifJpgBuilder::set_ascii_tag( ExifIfd ifd, ExifTag tag, const char* value)
{
    ExifEntry* entry = get_or_create_entry( ifd, tag );
    if (entry->data) {
        free(entry->data); // Libère l'ancienne valeur si le tag existait déjà
    }
    entry->format = EXIF_FORMAT_ASCII;
    entry->components = strlen(value) + 1;
    entry->size = entry->components;
    entry->data = static_cast<unsigned char*>(malloc(entry->size));
    memcpy(entry->data, value, entry->size);
}
//-------------------------------------------------------------------------------------------------------------------
//
// 2. Ajouter ou remplacer un tag SHORT (entier 16 bits, ex: Orientation, ISO)
//
//-------------------------------------------------------------------------------------------------------------------
void ExifJpgBuilder::set_short_tag( ExifIfd ifd, ExifTag tag, ExifShort value) {
    ExifEntry* entry = get_or_create_entry( ifd, tag);
    if (!entry->data) {
        exif_entry_initialize(entry, tag);
    }
    ExifByteOrder order = exif_data_get_byte_order(exif);
    exif_set_short(entry->data, order, value);
}
//-------------------------------------------------------------------------------------------------------------------
//
// 3. Ajouter ou remplacer un tag RATIONAL (fraction numérateur/dénominateur, ex: 1/250s, f/2.8)
//
//-------------------------------------------------------------------------------------------------------------------
void ExifJpgBuilder::set_rational_tag( ExifIfd ifd, ExifTag tag, ExifLong num, ExifLong den)
{
    ExifEntry* entry = get_or_create_entry( ifd, tag);
    if (!entry->data) {
        exif_entry_initialize(entry, tag);
    }
    ExifByteOrder order = exif_data_get_byte_order(exif);
    ExifRational val{num, den};
    exif_set_rational(entry->data, order, val);
}
//-------------------------------------------------------------------------------------------------------------------
//
//
//-------------------------------------------------------------------------------------------------------------------

