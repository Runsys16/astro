#ifndef EXIF_JPG_H
#define EXIF_JPG_H


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <jpeglib.h>
#include <libexif/exif-data.h>

using namespace std;



class ExifJpgBuilder
{
private:
	ExifData*		 			exif		 	= nullptr;

	jpeg_decompress_struct		srcinfo{};
	jpeg_error_mgr				jsrcerr{};
	jpeg_compress_struct		dstinfo{};
	jpeg_error_mgr				jdsterr{};

	unsigned char*				old_exif_data	= nullptr;
	unsigned int				old_exif_len	= 0;

	unsigned char*				out_buf 		= nullptr;
	unsigned long				out_size 		= 0;

	unsigned char*				new_exif_data	= nullptr;
	unsigned int				new_exif_len	= 0;

    vector<unsigned char>		out_jpeg;


public:
	bool			start( const vector<unsigned char>& );
	bool			stop();

	ExifEntry*		get_or_create_entry( ExifIfd ifd, ExifTag tag );
	void			set_ascii_tag( ExifIfd ifd, ExifTag tag, const char* value );
	void			set_short_tag( ExifIfd ifd, ExifTag tag, ExifShort value );
	void			set_rational_tag( ExifIfd ifd, ExifTag tag, ExifLong num, ExifLong den );
	
inline vector<unsigned char>&	get_out_buff()		{ return out_jpeg; }
};

#endif
