#ifndef _LKL_HOST_H
#define _LKL_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <lkl/asm/host_ops.h>
#include <lkl.h>

extern struct lkl_host_operations lkl_host_ops;

/**
 * lkl_printf - print a message via the host print operation
 *
 * @fmt - printf like format string
 */
int lkl_printf(const char *fmt, ...);

extern char lkl_virtio_devs[256];

#ifdef CONFIG_AUTO_LKL_POSIX_HOST
#include <sys/uio.h>
#else
struct iovec {
	void *iov_base;
	size_t iov_len;
};
#endif

extern struct lkl_dev_blk_ops lkl_dev_blk_ops;

#define LKL_DEV_BLK_TYPE_READ		0
#define LKL_DEV_BLK_TYPE_WRITE		1
#define LKL_DEV_BLK_TYPE_FLUSH		4
#define LKL_DEV_BLK_TYPE_FLUSH_OUT	5

struct lkl_blk_req {
	unsigned int type;
	unsigned int prio;
	unsigned long long sector;
	struct iovec *buf;
	int count;
};

struct lkl_dev_blk_ops {
	int (*get_capacity)(struct lkl_disk disk, unsigned long long *res);
#define LKL_DEV_BLK_STATUS_OK		0
#define LKL_DEV_BLK_STATUS_IOERR	1
#define LKL_DEV_BLK_STATUS_UNSUP	2
	int (*request)(struct lkl_disk disk, struct lkl_blk_req *req);
};

struct lkl_netdev {
	struct lkl_dev_net_ops *ops;
	uint8_t has_vnet_hdr: 1;
};

struct lkl_dev_net_ops {
	/*
	 * Writes a L2 packet into the net device.
	 *
	 * The data buffer can only hold 0 or 1 complete packets.
	 *
	 * @nd - pointer to the network device
	 * @iov - pointer to the buffer vector
	 * @cnt - # of vectors in iov.
	 * @returns number of bytes transmitted
	 */
	int (*tx)(struct lkl_netdev *nd, struct iovec *iov, int cnt);

	/*
	 * Reads a packet from the net device.
	 *
	 * It must only read one complete packet if present.
	 *
	 * If the buffer is too small for the packet, the implementation may
	 * decide to drop it or trim it.
	 *
	 * @nd - pointer to the network device
	 * @iov - pointer to the buffer vector to store the packet
	 * @cnt - # of vectors in iov.
	 * @returns number of bytes read for success or < 0 if error
	 */
	int (*rx)(struct lkl_netdev *nd, struct iovec *iov, int cnt);

#define LKL_DEV_NET_POLL_RX		1
#define LKL_DEV_NET_POLL_TX		2
#define LKL_DEV_NET_POLL_HUP		4

	/*
	 * Polls a net device.
	 *
	 * Supports the following events: LKL_DEV_NET_POLL_RX (readable),
	 * LKL_DEV_NET_POLL_TX (writable) or LKL_DEV_NET_POLL_HUP (the close
	 * operations has been issued and we need to clean up). Blocks until one
	 * event is available.
	 *
	 * @nd - pointer to the network device
	 */
	int (*poll)(struct lkl_netdev *nd);

	/*
	 * Make poll wakeup and return LKL_DEV_NET_POLL_HUP.
	 */
	void (*poll_hup)(struct lkl_netdev *nd);

	/*
	 * Frees a network device.
	 *
	 * Implementation must release its resources and free the network device
	 * structure.
	 */
	void (*free)(struct lkl_netdev *nd);
};

#ifdef __cplusplus
}
#endif

#endif
