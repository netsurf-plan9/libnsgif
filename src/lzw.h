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


/* Declare lzw internal context structure */
struct lzw_ctx;


/** LZW decoding response codes */
typedef enum lzw_result {
	LZW_OK,        /**< Success */
	LZW_OK_EOD,    /**< Success; reached zero-length sub-block */
	LZW_NO_MEM,    /**< Error: Out of memory */
	LZW_NO_DATA,   /**< Error: Out of data */
} lzw_result;


/**
 * Create an LZW decompression context.
 *
 * \param[out] ctx  Returns an LZW decompression context.  Caller owned,
 *                  free with lzw_context_destroy().
 * \return LZW_OK on success, or appropriate error code otherwise.
 */
lzw_result lzw_context_create(
		struct lzw_ctx **ctx);

/**
 * Destroy an LZW decompression context.
 *
 * \param[in] ctx  The LZW decompression context to destroy.
 */
void lzw_context_destroy(
		struct lzw_ctx *ctx);


#endif
