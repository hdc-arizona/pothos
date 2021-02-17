#pragma once

#include <iostream>

/******************************************************************************/

#define OK_OR_DIE(exp)                                                  \
do {                                                                    \
  if (!(exp).ok()) {                                                    \
    std::cerr << "DIE AT " << __FILE__ << ":" << __LINE__ << std::endl; \
    exit(1);                                                            \
  }                                                                     \
} while (0);

#define DIE_WHEN(exp)                                                   \
do {                                                                    \
  if (exp) {                                                            \
    std::cerr << "DIE AT " << __FILE__ << ":" << __LINE__ << std::endl; \
    exit(1);                                                            \
  }                                                                     \
} while (0);

/******************************************************************************/
