/*
 * This file is part of NetSurf's LibNSGIF, http://www.netsurf-browser.org/
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 *
 * Copyright 2017 Michael Drake <michael.drake@codethink.co.uk>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lzw.h"

/**
 * \file
 * \brief LZW decompression (implementation)
 *
 * Decoder for GIF LZW data.
 */


/**
 * Context for reading LZW data.
 *
 * LZW data is split over multiple sub-blocks.  Each sub-block has a
 * byte at the start, which says the sub-block size, and then the data.
 * Zero-size sub-blocks have no data, and the biggest sub-block size is
 * 255, which means there are 255 bytes of data following the sub-block
 * size entry.
 *
 * Note that an individual LZW code can be split over up to three sub-blocks.
 */
struct lzw_read_ctx {
	const uint8_t *data;    /**< Pointer to start of input data */
	uint32_t data_len;      /**< Input data length */
	uint32_t data_sb_next;  /**< Offset to sub-block size */

	const uint8_t *sb_data; /**< Pointer to current sub-block in data */
	uint32_t sb_bit;        /**< Current bit offset in sub-block */
	uint32_t sb_bit_count;  /**< Bit count in sub-block */
};


/**
 * Advance the context to the next sub-block in the input data.
 *
 * \param[in] ctx  LZW reading context, updated on success.
 * \return LZW_OK or LZW_OK_EOD on success, appropriate error otherwise.
 */
static lzw_result lzw__block_advance(struct lzw_read_ctx *ctx)
{
	uint32_t block_size;
	uint32_t next_block_pos = ctx->data_sb_next;
	const uint8_t *data_next = ctx->data + next_block_pos;

	if (next_block_pos >= ctx->data_len) {
		return LZW_NO_DATA;
	}

	block_size = *data_next;

	if ((next_block_pos + block_size) >= ctx->data_len) {
		return LZW_NO_DATA;
	}

	ctx->sb_bit = 0;
	ctx->sb_bit_count = block_size * 8;

	if (block_size == 0) {
		ctx->data_sb_next += 1;
		return LZW_OK_EOD;
	}

	ctx->sb_data = data_next + 1;
	ctx->data_sb_next += block_size + 1;

	return LZW_OK;
}


/**
 * Get the next LZW code of given size from the raw input data.
 *
 * Reads codes from the input data stream coping with GIF data sub-blocks.
 *
 * \param[in]  ctx        LZW reading context, updated.
 * \param[in]  code_size  Size of LZW code to get from data.
 * \param[out] code_out   Returns an LZW code on success.
 * \return LZW_OK or LZW_OK_EOD on success, appropriate error otherwise.
 */
static inline lzw_result lzw__next_code(
		struct lzw_read_ctx *ctx,
		uint8_t code_size,
		uint32_t *code_out)
{
	uint32_t code = 0;
	uint8_t current_bit = ctx->sb_bit & 0x7;
	uint8_t byte_advance = (current_bit + code_size) >> 3;

	if (ctx->sb_bit + code_size < ctx->sb_bit_count) {
		/* Fast path: code fully inside this sub-block */
		const uint8_t *data = ctx->sb_data + (ctx->sb_bit >> 3);
		switch (byte_advance) {
			case 2: code |= data[2] << 16;
			case 1: code |= data[1] <<  8;
			case 0: code |= data[0] <<  0;
		}
		ctx->sb_bit += code_size;
	} else {
		/* Slow path: code spans sub-blocks */
		uint8_t byte = 0;
		uint8_t bits_remaining_0 = (code_size < (8 - current_bit)) ?
				code_size : (8 - current_bit);
		uint8_t bits_remaining_1 = code_size - bits_remaining_0;
		uint8_t bits_used[3] = {
			[0] = bits_remaining_0,
			[1] = bits_remaining_1 < 8 ? bits_remaining_1 : 8,
			[2] = bits_remaining_1 - 8,
		};

		while (true) {
			const uint8_t *data = ctx->sb_data;
			lzw_result res;

			/* Get any data from end of this sub-block */
			while (byte <= byte_advance &&
					ctx->sb_bit < ctx->sb_bit_count) {
				code |= data[ctx->sb_bit >> 3] << (byte << 3);
				ctx->sb_bit += bits_used[byte];
				byte++;
			}

			/* Check if we have all we need */
			if (byte > byte_advance) {
				break;
			}

			/* Move to next sub-block */
			res = lzw__block_advance(ctx);
			if (res != LZW_OK) {
				return res;
			}
		}
	}

	*code_out = (code >> current_bit) & ((1 << code_size) - 1);
	return LZW_OK;
}
