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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "circularbuf.h"

#define min(x,y) ({ 			\
		typeof(x) _x = (x);	    \
		typeof(y) _y = (y);	    \
		(void) (&_x == &_y);	\
		_x < _y ? _x : _y; })

#define roundup_pow_of_two(n)   \
    (1UL    <<                             \
        (                                  \
            (                              \
            (n) & (1UL << 31) ? 31 :       \
            (n) & (1UL << 30) ? 30 :       \
            (n) & (1UL << 29) ? 29 :       \
            (n) & (1UL << 28) ? 28 :       \
            (n) & (1UL << 27) ? 27 :       \
            (n) & (1UL << 26) ? 26 :       \
            (n) & (1UL << 25) ? 25 :       \
            (n) & (1UL << 24) ? 24 :       \
            (n) & (1UL << 23) ? 23 :       \
            (n) & (1UL << 22) ? 22 :       \
            (n) & (1UL << 21) ? 21 :       \
            (n) & (1UL << 20) ? 20 :       \
            (n) & (1UL << 19) ? 19 :       \
            (n) & (1UL << 18) ? 18 :       \
            (n) & (1UL << 17) ? 17 :       \
            (n) & (1UL << 16) ? 16 :       \
            (n) & (1UL << 15) ? 15 :       \
            (n) & (1UL << 14) ? 14 :       \
            (n) & (1UL << 13) ? 13 :       \
            (n) & (1UL << 12) ? 12 :       \
            (n) & (1UL << 11) ? 11 :       \
            (n) & (1UL << 10) ? 10 :       \
            (n) & (1UL <<  9) ?  9 :       \
            (n) & (1UL <<  8) ?  8 :       \
            (n) & (1UL <<  7) ?  7 :       \
            (n) & (1UL <<  6) ?  6 :       \
            (n) & (1UL <<  5) ?  5 :       \
            (n) & (1UL <<  4) ?  4 :       \
            (n) & (1UL <<  3) ?  3 :       \
            (n) & (1UL <<  2) ?  2 :       \
            (n) & (1UL <<  1) ?  1 :       \
            (n) & (1UL <<  0) ?  0 : -1    \
            ) + 1                          \
        )                                  \
)

static inline __attribute__((const))
int is_power_of_2(unsigned long n)
{
    return (n != 0 && ((n & (n - 1)) == 0));
}

static unsigned int __put(struct circularbuf_s *circularbuf, const void *from, unsigned int len)
{
    if (!circularbuf)
        return 0;

    if (!from || !len)
        return 0;

    unsigned int l,off;

    len = min(circularbuf->size - (circularbuf->in - circularbuf->out), len);

    off = (circularbuf->in + 0) & (circularbuf->size - 1);

    l = min(len, circularbuf->size - off);

    memcpy(circularbuf->buffer + off, from, l);
    memcpy(circularbuf->buffer, (char *)from + l, len - l);

    circularbuf->in += len;

    return len;
}

static unsigned int __get(struct circularbuf_s *circularbuf, void *to, unsigned int len)
{
    if (!circularbuf)
        return 0;

    if (!to || !len)
        return 0;

    unsigned int l,off;

    len = min(circularbuf->in - circularbuf->out, len);

    off = (circularbuf->out + 0) & (circularbuf->size - 1);

    l = min(len, circularbuf->size - off);
    memcpy(to, circularbuf->buffer + off, l);

    memcpy((char *)to + l, circularbuf->buffer, len - l);

    circularbuf->out += len;

    return len;
}

int circularbuf_create(struct circularbuf_s *cir, unsigned int size)
{
    if (!cir)
        return -1;

    if (!size)
        return -1;

    if (is_power_of_2(size))
        size = roundup_pow_of_two(size);

    cir->buffer = malloc(size);
    if (!cir->buffer)
        return -1;

    cir->size = size;
    cir->in = 0;
    cir->out = 0;

    cir->put = __put;
    cir->get = __get;

    return 0;
}

int circularbuf_destroy(struct circularbuf_s *circularbuf)
{
    if (!circularbuf)
        return -1;

    if (circularbuf->buffer) {
        free(circularbuf->buffer);
        circularbuf->buffer = NULL;
    }

    circularbuf->size = 0;
    circularbuf->in = 0;
    circularbuf->out = 0;

    return 0;
}
