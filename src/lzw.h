/*
 * This file is part of NetSurf's LibNSGIF, http://www.netsurf-browser.org/
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 *
 * Copyright 2017 Michael Drake <michael.drake@codethink.co.uk>
 */

#ifndef LZW_H_
#define LZW_H_

/**
 * \file
 * \brief LZW decompression (interface)
 *
 * Decoder for GIF LZW data.
 */


/** LZW decoding response codes */
typedef enum lzw_result {
	LZW_OK,        /**< Success */
	LZW_OK_EOD,    /**< Success; reached zero-length sub-block */
	LZW_NO_DATA,   /**< Error: Out of data */
} lzw_result;


#endif
