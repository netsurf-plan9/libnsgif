/*
 * Copyright 2004 Richard Wilson <not_ginger_matt@users.sourceforge.net>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
 * Progressive animated GIF file decoding (interface).
 */

#ifndef _LIBNSGIF_H_
#define _LIBNSGIF_H_

#include <stdbool.h>

/*	Error return values
*/
typedef enum {
	GIF_WORKING = 1,
	GIF_OK = 0,
	GIF_INSUFFICIENT_FRAME_DATA = -1,
	GIF_FRAME_DATA_ERROR = -2,
	GIF_INSUFFICIENT_DATA = -3,
	GIF_DATA_ERROR = -4,
	GIF_INSUFFICIENT_MEMORY = -5,
	GIF_FRAME_NO_DISPLAY = -6
} gif_result;

/*	The GIF frame data
*/
typedef struct gif_frame {
  	unsigned int frame_pointer;		/**< offset (in bytes) to the GIF frame data */
  	unsigned int frame_delay;		/**< delay (in cs) before animating the frame */
  	bool display;				/**< whether the frame should be displayed/animated */
  	bool virgin;				/**< whether the frame has previously been used */
	bool opaque;				/**< whether the frame is totally opaque */
	bool redraw_required;			/**< whether a forcable screen redraw is required */
	unsigned char disposal_method;		/**< how the previous frame should be disposed; affects plotting */
	bool transparency;	 		/**< whether we acknoledge transparency */
	unsigned char transparency_index;	/**< the index designating a transparent pixel */
	unsigned int redraw_x;			/**< x co-ordinate of redraw rectangle */
	unsigned int redraw_y;			/**< y co-ordinate of redraw rectangle */
	unsigned int redraw_width;		/**< width of redraw rectangle */
	unsigned int redraw_height;		/**< height of redraw rectangle */
} gif_frame;

/*	API for Bitmap callbacks
*/
typedef void* (*gif_bitmap_cb_create)(int width, int height);
typedef void (*gif_bitmap_cb_destroy)(void *bitmap);
typedef char* (*gif_bitmap_cb_get_buffer)(void *bitmap);
typedef void (*gif_bitmap_cb_set_opaque)(void *bitmap, bool opaque);
typedef bool (*gif_bitmap_cb_test_opaque)(void *bitmap);
typedef void (*gif_bitmap_cb_modified)(void *bitmap);

/*	The Bitmap callbacks function table
*/
typedef struct gif_bitmap_callback_vt_s {
	gif_bitmap_cb_create bitmap_create;		/**< Create a bitmap. */
	gif_bitmap_cb_destroy bitmap_destroy;		/**< Free a bitmap. */
	gif_bitmap_cb_get_buffer bitmap_get_buffer;	/**< Return a pointer to the pixel data in a bitmap. */
	gif_bitmap_cb_set_opaque bitmap_set_opaque;	/**< Sets whether a bitmap should be plotted opaque. */
	gif_bitmap_cb_test_opaque bitmap_test_opaque;	/**< Tests whether a bitmap has an opaque alpha channel. */
	gif_bitmap_cb_modified bitmap_modified;		/**< The bitmap image has changed, so flush any persistant cache. */
} gif_bitmap_callback_vt;

/*	The GIF animation data
*/
typedef struct gif_animation {
	unsigned char *gif_data;		/**< pointer to GIF data */
	unsigned int buffer_position;		/**< current index into GIF data */
	unsigned int buffer_size;		/**< total number of bytes of GIF data available */
	unsigned int frame_holders;		/**< current number of frame holders */
	int decoded_frame;			/**< current frame decoded to bitmap */
	int loop_count;				/**< number of times to loop animation */
	gif_frame *frames;			/**< decoded frames */
	unsigned int width;			/**< width of GIF (may increase during decoding) */
	unsigned int height;			/**< heigth of GIF (may increase during decoding) */
	unsigned int frame_count;		/**< number of frames decoded */
	unsigned int frame_count_partial;	/**< number of frames partially decoded */
	unsigned int background_colour;	/**< image background colour */
	unsigned int aspect_ratio;		/**< image aspect ratio (ignored) */
	unsigned int colour_table_size;	/**< size of colour table (in entries) */
	bool global_colours;			/**< whether the GIF has a global colour table */
	unsigned int *global_colour_table;	/**< global colour table */
	unsigned int *local_colour_table;	/**< local colour table */
	int dirty_frame;			/**< the current dirty frame, or -1 for none */
	void *frame_image;			/**< currently decoded image; stored as bitmap from bitmap_create callback */
	gif_result current_error;		/**< current error type, or 0 for none*/
} gif_animation;

gif_result gif_initialise(struct gif_animation *gif, gif_bitmap_callback_vt *bitmap_callbacks);
gif_result gif_decode_frame(struct gif_animation *gif, unsigned int frame, gif_bitmap_callback_vt *bitmap_callbacks);
void gif_finalise(struct gif_animation *gif, gif_bitmap_callback_vt *bitmap_callbacks);

#endif
