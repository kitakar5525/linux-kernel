/*
 * videobuf2-dma-contig.h - DMA contig memory allocator for videobuf2
 *
 * Copyright (C) 2010 Samsung Electronics
 *
 * Author: Pawel Osciak <pawel@osciak.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 */

#ifndef _MEDIA_VIDEOBUF2_DMA_CONTIG_H
#define _MEDIA_VIDEOBUF2_DMA_CONTIG_H

#include <media/videobuf2-v4l2.h>
#include <linux/dma-mapping.h>
struct vb2_dc_buf {
	struct device			*dev;
	void				*vaddr;
	unsigned long			size;
	dma_addr_t			dma_addr;
	enum dma_data_direction		dma_dir;
	struct sg_table			*dma_sgt;

	/* MMAP related */
	struct vb2_vmarea_handler	handler;
	atomic_t			refcount;
	struct sg_table			*sgt_base;

	/* USERPTR related */
	struct vm_area_struct		*vma;

	/* DMABUF related */
	struct dma_buf_attachment	*db_attach;
};


static inline dma_addr_t
vb2_dma_contig_plane_dma_addr(struct vb2_buffer *vb, unsigned int plane_no)
{
	dma_addr_t *addr = vb2_plane_cookie(vb, plane_no);

	return *addr;
}

void *vb2_dma_contig_init_ctx(struct device *dev);
void vb2_dma_contig_cleanup_ctx(void *alloc_ctx);

extern const struct vb2_mem_ops vb2_dma_contig_memops;

#endif
