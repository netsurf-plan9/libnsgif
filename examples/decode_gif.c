/*
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
 * Copyright 2008 James Bursa <james@netsurf-browser.org>
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

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libnsgif.h>

char *load_file(const char *path, size_t *data_size);
void warning(const char *context, int code);
void *bitmap_create(int width, int height);
void bitmap_set_opaque(void *bitmap, bool opaque);
bool bitmap_test_opaque(void *bitmap);
char *bitmap_get_buffer(void *bitmap);
void bitmap_destroy(void *bitmap);
void bitmap_modified(void *bitmap);


gif_bitmap_callback_vt bitmap_callbacks = {
	bitmap_create,
	bitmap_destroy,
	bitmap_get_buffer,
	bitmap_set_opaque,
	bitmap_test_opaque,
	bitmap_modified
};


int main(int argc, char *argv[])
{
	struct gif_animation gif;
	size_t size;
	int code;
	unsigned int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s image.gif\n", argv[0]);
		return 1;
	}

	/* load file into memory */
	char *data = load_file(argv[1], &size);

	/* initialise gif_animation structure */
	gif.buffer_size = size;
	gif.gif_data = (unsigned char *) data;
	gif.buffer_position = 0;

	/* begin decoding */
	do {
		code = gif_initialise(&gif, &bitmap_callbacks);
		if (code != 0 && code != 1)
			warning("gif_initialise", code);
	} while (code != 1);

	printf("P3\n");
	printf("# %s\n", argv[1]);
	printf("# width                %u \n", gif.width);
	printf("# height               %u \n", gif.height);
	printf("# frame_count          %u \n", gif.frame_count);
	printf("# frame_count_partial  %u \n", gif.frame_count_partial);
	printf("# background_colour    %x \n", gif.background_colour);
	printf("# loop_count           %u \n", gif.loop_count);
	printf("%u %u 256\n", gif.width, gif.height * gif.frame_count);

	/* decode the frames */
	for (i = 0; i != gif.frame_count; i++) {
		unsigned int row, col;
		char *image;

		code = gif_decode_frame(&gif, i, &bitmap_callbacks);
		if (code != 0)
			warning("gif_decode_frame", code);

		printf("# frame %u:\n", i);
		image = (char *) gif.frame_image;
		for (row = 0; row != gif.height; row++) {
			for (col = 0; col != gif.width; col++) {
				size_t z = (row * gif.width + col) * 4;
				printf("%u %u %u ",
					(unsigned char) image[z],
					(unsigned char) image[z + 1],
					(unsigned char) image[z + 2]);
			}
			printf("\n");
		}
	}

	return 0;
}


char *load_file(const char *path, size_t *data_size)
{
	FILE *fd;
	struct stat sb;
	char *buffer;
	size_t size;
	size_t n;

	fd = fopen(path, "rb");
	if (!fd) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	if (stat(path, &sb)) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	size = sb.st_size;

	buffer = malloc(size);
	if (!buffer) {
		fprintf(stderr, "Unable to allocate %lld bytes\n",
				(long long) size);
		exit(EXIT_FAILURE);
	}

	n = fread(buffer, 1, size, fd);
	if (n != size) {
		perror(path);
		exit(EXIT_FAILURE);
	}

	fclose(fd);

	*data_size = size;
	return buffer;
}


void warning(const char *context, int code)
{
	fprintf(stderr, "%s failed: ", context);
	switch (code)
	{
	case GIF_INSUFFICIENT_FRAME_DATA:
		fprintf(stderr, "GIF_INSUFFICIENT_FRAME_DATA");
		break;
	case GIF_FRAME_DATA_ERROR:
		fprintf(stderr, "GIF_FRAME_DATA_ERROR");
		break;
	case GIF_INSUFFICIENT_DATA:
		fprintf(stderr, "GIF_INSUFFICIENT_DATA");
		break;
	case GIF_DATA_ERROR:
		fprintf(stderr, "GIF_DATA_ERROR");
		break;
	case GIF_INSUFFICIENT_MEMORY:
		fprintf(stderr, "GIF_INSUFFICIENT_MEMORY");
		break;
	default:
		fprintf(stderr, "unknown code %i", code);
		break;
	}
	fprintf(stderr, "\n");
}


void *bitmap_create(int width, int height)
{
	return calloc(width * height, 4);
}


void bitmap_set_opaque(void *bitmap, bool opaque)
{
	(void) opaque;  /* unused */
	assert(bitmap);
}


bool bitmap_test_opaque(void *bitmap)
{
	assert(bitmap);
	return false;
}


char *bitmap_get_buffer(void *bitmap)
{
	assert(bitmap);
	return bitmap;
}


void bitmap_destroy(void *bitmap)
{
	assert(bitmap);
	free(bitmap);
}


void bitmap_modified(void *bitmap)
{
	assert(bitmap);
	return;
}

