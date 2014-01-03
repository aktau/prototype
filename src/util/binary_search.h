/**
 * This file is part of prototype.
 *
 * original source taken from this article:
 * http://www.pvk.ca/Blog/2012/07/03/binary-search-star-eliminates-star-branch-mispredictions/
 *
 * (c) 2012 Paul Khuong <http://www.pvk.ca/>
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#ifndef __binary_search_h__
#define __binary_search_h__

/* this is one of those few times where I'd like C++'s templates..., actually
 * just C11 + templates would be an awesome language */

/**
 * log2 ceiling (lower bound)
 *
 * __builtin_clz:
 *   returns the number of leading 0-bits in x, starting at the most significant bit position. If x is 0, the result is undefined.
 * __builtin_ctz:
 *   returns the number of trailing 0-bits in x, starting at the least significant bit position. If x is 0, the result is undefined.
 */
static ALWAYS_INLINE unsigned int lb(size_t x) {
    if (x <= 1) return 0;

    return (8 * sizeof(x)) - (unsigned int) __builtin_clzl(x - 1);
}

/* size must be a power of 2 */
static ALWAYS_INLINE ssize_t binary_search(uint64_t key, uint64_t *array, size_t size) {
    uint64_t *low = array;

    /* this should unroll... check this manually */
    for (unsigned int i = lb(size); i != 0; --i) {
        size /= 2;

        uint64_t mid = low[size];

        if (mid <= key) {
            low += size;
        }
    }

    return (*low > key) ? -1 : (low - array);
}

#endif
