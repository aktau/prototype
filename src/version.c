/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include <stdio.h>

#include "util.h"

const char *wfCompiler(void) {
#if defined(__clang__) && defined(__clang_major__) && defined(__clang_minor__) && defined(__clang_patchlevel__)
  return "clang " XSTR(__clang_major__) "." XSTR(__clang_minor__) "." XSTR(__clang_patchlevel__);
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
  return "gcc " XSTR(__GNUC__) "." XSTR(__GNUC_MINOR__) "." XSTR(__GNUC_PATCHLEVEL__);
#else
  return "unknown compiler";
#endif
}
