/**
 * @file nxp_simtemp_helpers.c
 * @brief Implementation for the nxp_simtemp.
 *
 * This file contains the definitions for helper functions.
 * Some of these helper functions are.
 * > Get time in iso8601 format.
 * > Generate temperature simulation.
 *
 * @author Jose Jorge Figueroa
 * @author https://github.com/Figuejojo
 * @date 2025-09-29
 */
/***********************************************
 *  Includes
 ***********************************************/
#include "nxp_simtemp_helpers.h"

/***********************************************
 *  Definitions
 ***********************************************/
#define TEMP_GAUSS_MEAN_mC          (30000)
#define TEMP_GAUSS_SIGMA_NORMAL_mC  (3000)

/***********************************************
 *  Static Function Prototypes
 ***********************************************/
static long get_gauss_rand_temp_mC(long mean_milli, long std_dev_milli);

/***********************************************
 *  Static Functions
 ***********************************************/
/**
 * @details Generate a pseudo-Gaussian random number in milliCelsius using the
 * Central Limit Theorem and integer arithmetic.
 *
 * This function sums 12 uniform random numbers to approximate a Gaussian
 * distribution, then scales the result to the desired mean and standard
 * deviation. - Based on Central Limit Theorem.
 *
 * @param mean_milli    Mean temperature in milliCelsius.
 * @param std_dev_milli Standard deviation in milliCelsius.
 *
 * @return Temperature value in milliCelsius
 */
static long get_gauss_rand_temp_mC(long mean_milli, long std_dev_milli)
{
  int i;
  unsigned int u_rand;
  long long sum64 = 0;
  long rand_norm;
  const int scaling_bits = 24; // Precision

  // Sum 12 random unsigned integers.
  // The sum will be approximately Gaussian, centered around 6 * 2^32.
  // We use `long long` for `sum64` to prevent overflow.
  for (i = 0; i < 12; i++)
    {
    get_random_bytes(&u_rand, sizeof(u_rand));
    sum64 += u_rand;
  }

  // Normalize the sum to a standard normal distribution (mean 0, std_dev 1).
  // We shift the sum and subtract the calculated mean to center it at zero.
  // The mean of `sum64` is 12 * (2^32 / 2) = 6 * 2^32.
  // The normalization constant is derived from the standard deviation.
  // We subtract the mean `6LL << 32` and scale down.
  // By shifting down, we get our fixed-point representation.
  rand_norm = (long)((sum64 - (6LL << 32)) >> scaling_bits);

  // The standard deviation of `rand_norm` is now `(2^32 * sqrt(12)) / 2^scaling_bits`.
  // We rescale this to match the desired `std_dev_milli`.
  // Since sqrt(12) is roughly 3.46, the std_dev of `rand_norm` is `3.46 * 2^(32-scaling_bits)`.
  // To match `std_dev_milli`, we need to multiply by `std_dev_milli / (3.46 * 2^(32-scaling_bits))`.
  // A simpler, though less precise, approximation is to multiply and scale.
  // rand_norm is now roughly a standard normal. We scale it up.
  // A multiplication followed by a bit shift is a fixed-point multiplication.
  rand_norm = (rand_norm * std_dev_milli) >> (32 - scaling_bits + 2); // Shift adjusted for better scaling

  // Add the desired mean.
  return rand_norm + mean_milli;
}

/***********************************************
 *  Functions
 ***********************************************/
/**
 * @details Produce a random temperature basedo on `#TEMP_GAUSS_MEAN_mC`
 *          and `#TEMP_GAUSS_SIGMA_NORMAL_mC`
 */
long get_normal_temperature_mC(void)
{
  // Simulate a temperature reading with a mean of 25.0°C and std dev of 3.0°C
  return get_gauss_rand_temp_mC(TEMP_GAUSS_MEAN_mC, TEMP_GAUSS_SIGMA_NORMAL_mC);
}

/**
 * @details Timestamp format as Format time as ISO-8601 UTC
 *            with milliseconds
 */
void ts_iso8601_now(char *buf, size_t buflen)
{
  struct timespec64 ts;
  struct tm tm;
  u32 msec;

  ktime_get_real_ts64(&ts); // Real Clock (UTC)
  time64_to_tm(ts.tv_sec, 0, &tm); //Seconds to UTC Break-down
  msec = ts.tv_nsec / 1000000U;

  // YYYY-MM-DDTHH:MM:SS.mmmZ
  snprintf(buf, buflen, "%04ld-%02d-%02dT%02d:%02d:%02d.%03uZ",
          (long)tm.tm_year + 1900,
          tm.tm_mon + 1,
          tm.tm_mday,
          tm.tm_hour,
          tm.tm_min,
          tm.tm_sec,
          msec);
}