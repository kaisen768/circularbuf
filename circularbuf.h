/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Jz.
 *       Filename:  circularbuf.c
 *
 *    Description:  环形缓冲区的实现
 *         Others:  1.min的妙用，(验证剩余有效空间和要求要读出或者写入空间 取最小值)
 *                  2.利用unsigned int 的回环,in 和 out一直在加，加到0xffffffff则归为0，任然满足计算偏移等。
 *                  3.分为2部进行copy，一为当前偏移到size-1 二为剩余部分0到(len减去一中的个数)
 *                  4.unsiged int下的(in - out)始终为in和out之间的距离，(in溢出后in:0x1 - out:0xffffffff = 2任然满足)(缓冲区中未脏的数据).
 *                  5.计算偏移(in) & (size - 1) <==> in%size
 *        Version:  1.0
 *        Date:  Monday, November 28, 2020 11:30:00 BJT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Joy (Joy), 
 *   Organization:  jz
 *        History:   Created by houkaisen
 *
 *           note:  参照原代码url - https://github.com/houwentaoff/kfifo
 * =====================================================================================
 */
#ifndef _CIRCULARBUF_H_
#define _CIRCULARBUF_H_

typedef struct circularbuf_s circularbuf_t;

struct circularbuf_s {
    unsigned char *buffer;  /* the buffer holding the data */
    unsigned int size;      /* the size of the allocated buffer */
    unsigned int in;        /* data is added at offset (in % size) */
    unsigned int out;       /* data is extracted from off. (out % size) */

    /** 
     * put - puts some data into the circularbuf, no locking version
     * @circularbuf: the circularbuf to be used.
     * @from: the data to be added.
     * @len: the length of the data to be added.
     * 
     * This function copies at most @len bytes from the @buffer into
     * the circularbuf depending on the free space, and returns the number of
     * bytes copied.
     * 
     * @note that with only one concurrent reader and one concurrent
     * writer, you don't need extra locking to use these functions.
     */
    unsigned int (*put)(struct circularbuf_s *circularbuf, const void *from, unsigned int len);
    
    /** 
     * get - gets some data from the circularbuf, no locking version
     *  @circularbuf: the circularbuf to be used.
     *  @to: where the data must be copied.
     *  @len: the size of the destination buffer.
     * 
     *  This function copies at most @len bytes from the circularbuf into the
     *  @buffer and returns the number of copied bytes.
     * 
     *  @note that with only one concurrent reader and one concurrent
     *  writer, you don't need extra locking to use these functions.
     */
    unsigned int (*get)(struct circularbuf_s *circularbuf, void *to, unsigned int len);
};

/**
 * @brief create circular buffer
 * 
 * @param cir circular buffer structure
 * @param size size of circular buffer
 *      @note size must is a number pow of two
 * @return 0 if success, or -1 if an error occurred
 */
int circularbuf_create(struct circularbuf_s *cir, unsigned int size);

/**
 * @brief destroy circular buffer
 * 
 * @param cir circular buffer structure
 * @return 0 if success, or -1 if an error occurred
 */
int circularbuf_destroy(struct circularbuf_s *cir);

#endif
