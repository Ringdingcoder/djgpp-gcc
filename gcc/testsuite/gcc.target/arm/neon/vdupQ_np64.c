/* Test the `vdupQ_np64' ARM Neon intrinsic.  */
/* This file was autogenerated by neon-testgen.  */

/* { dg-do assemble } */
/* { dg-require-effective-target arm_crypto_ok } */
/* { dg-options "-save-temps -O0" } */
/* { dg-add-options arm_crypto } */

#include "arm_neon.h"

void test_vdupQ_np64 (void)
{
  poly64x2_t out_poly64x2_t;
  poly64_t arg0_poly64_t;

  out_poly64x2_t = vdupq_n_p64 (arg0_poly64_t);
}

/* { dg-final { cleanup-saved-temps } } */
