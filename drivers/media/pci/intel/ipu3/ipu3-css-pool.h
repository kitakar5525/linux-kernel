/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright (C) 2018 Intel Corporation */

#ifndef __IPU3_UTIL_H
#define __IPU3_UTIL_H

struct device;
struct imgu_device;

#define IPU3_CSS_POOL_SIZE		4

/**
 * ipu3_css_map - store DMA mapping info for buffer
 *
 * @size:		size of the buffer in bytes.
 * @vaddr:		kernel virtual address.
 * @daddr:		iova dma address to access IPU3.
 * @vma:		private, a pointer to &struct vm_struct,
 *			used for ipu3_dmamap_free.
 */
struct ipu3_css_map {
	size_t size;
	void *vaddr;
	dma_addr_t daddr;
	struct vm_struct *vma;
};

/**
 * ipu3_css_pool - circular buffer pool definition
 *
 * @entry:		array with IPU3_CSS_POOL_SIZE elements.
 * @entry.param:	a &struct ipu3_css_map for storing the mem mapping.
 * @entry.framenum:	the css frame number, used to determine if the entry
 *			is old enough to be recycled.
 * @last:		write pointer, initialized to IPU3_CSS_POOL_SIZE.
 */
struct ipu3_css_pool {
	struct {
		struct ipu3_css_map param;
		long framenum;
	} entry[IPU3_CSS_POOL_SIZE];
	unsigned int last; /* Latest entry */
};

int ipu3_css_dma_buffer_resize(struct imgu_device *imgu,
			       struct ipu3_css_map *map, size_t size);
void ipu3_css_pool_cleanup(struct imgu_device *imgu,
			   struct ipu3_css_pool *pool);
int ipu3_css_pool_init(struct imgu_device *imgu, struct ipu3_css_pool *pool,
		       size_t size);
int ipu3_css_pool_get(struct ipu3_css_pool *pool, long framenum);
void ipu3_css_pool_put(struct ipu3_css_pool *pool);
const struct ipu3_css_map *ipu3_css_pool_last(struct ipu3_css_pool *pool,
					      unsigned int last);

#endif
