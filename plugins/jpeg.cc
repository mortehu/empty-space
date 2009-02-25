/***************************************************************************
                             jpeg.cc  -  Interface to libjpeg
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>

#include <espace/file.h>
#include <espace/image.h>
#include <espace/output.h>

#include "jpeg.h"

extern "C"
{
#define XMD_H
#include <jpeglib.h>
}

static void init_source (j_decompress_ptr cinfo)
{
  return;
}

static boolean fill_input_buffer (j_decompress_ptr cinfo)
{
  return 0;
}

static void skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  cinfo->src->next_input_byte += num_bytes;
  cinfo->src->bytes_in_buffer -= num_bytes;
}

static void term_source (j_decompress_ptr cinfo)
{
  return;
}

uint32_t JPEG::id()
{
  return 0x4A464946; // "JFIF"
}

bool JPEG::canHandle(File& file)
{
  if(file.length() < 10)
    return false;

  char magic[4];

  file.seek(6);
  file.read(magic, 4);

  return !memcmp(magic, "JFIF", 4);
}

Image* JPEG::read(File& file)
{
  struct jpeg_error_mgr errorManager;
  struct jpeg_decompress_struct decompressInfo;

  decompressInfo.err = jpeg_std_error(&errorManager);

  jpeg_create_decompress(&decompressInfo);

  struct jpeg_source_mgr srcmgr;

  srcmgr.next_input_byte = file.data();
  srcmgr.bytes_in_buffer = file.length();
  srcmgr.init_source = init_source;
  srcmgr.fill_input_buffer = fill_input_buffer;
  srcmgr.skip_input_data = skip_input_data;
  srcmgr.resync_to_restart = jpeg_resync_to_restart;
  srcmgr.term_source = term_source;

  decompressInfo.src = &srcmgr;

  jpeg_read_header(&decompressInfo, TRUE);

  jpeg_start_decompress(&decompressInfo);

  uint pixelFormat;

  switch(decompressInfo.output_components)
  {
  case 1: pixelFormat = Image::Gray; break;
  case 3: pixelFormat = Image::RGB; break;
  case 4: pixelFormat = Image::RGBA; break;

  default:

    jpeg_destroy_decompress(&decompressInfo);

    esWarning << "JPEG: Unsupported pixel format." << std::endl;

    return 0;
  }

  Image* image = new Image(decompressInfo.output_width,
                           decompressInfo.output_height,
                           pixelFormat);

  uint pitch = decompressInfo.output_width
             * decompressInfo.output_components;

  while(decompressInfo.output_scanline < decompressInfo.output_height)
  {
    uint8_t* ptr = image->data() + decompressInfo.output_scanline * pitch;

    jpeg_read_scanlines(&decompressInfo, (JSAMPLE**) &ptr, 1);
  }

  jpeg_finish_decompress(&decompressInfo);

  jpeg_destroy_decompress(&decompressInfo);

  return image;
}

// vim: ts=2 sw=2 et
