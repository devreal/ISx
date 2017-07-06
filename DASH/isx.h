/*
Copyright (c) 2015, Intel Corporation

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:

    * Redistributions of source code must retain the above copyright 
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above 
      copyright notice, this list of conditions and the following 
      disclaimer in the documentation and/or other materials provided 
      with the distribution.
    * Neither the name of Intel Corporation nor the names of its 
      contributors may be used to endorse or promote products 
      derived from this software without specific prior written 
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _ISX_H
#define _ISX_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <libdash.h>
#include "timer.h"
#include "pcg_basic.h"
/*
 * Ensures the command line parameters and values specified in params.h
 * are valid and will not cause problems.
 */
static char * parse_params(const int argc, char ** argv);

/*
 * Sorts a random array by distrectizing the domain of values into buckets.
 * Each bucket is assigned to a PE and all keys belonging to a bucket are sent
 * to the corresponding PE. Each PE then performs a local sort of the keys in its bucket.
 */
static int bucket_sort();

#ifdef PERMUTE
/*
 * Creates a randomly ordered array of PEs used in the exchange_keys function
 */
static void create_permutation_array();

/*
 * Randomly shuffles a generic array
 */
static void shuffle(void * array, size_t n, size_t size);
#endif


/*
 * Generates random keys [0, MAX_KEY_VAL] on each rank using the time and rank as a seed
 */
static inline std::vector<KEY_TYPE> make_input(void);

/*
 * Computes the size of each local bucket by iterating all local keys and incrementing
 * their corresponding bucket's size
 */
static inline int * count_local_bucket_sizes(
  const std::vector<KEY_TYPE> &my_keys,
  dash::NArray<int, 2>        &bucket_sizes);

/*
 * Rearranges all local keys into their corresponding local bucket.
 * The contents of each bucket are not sorted.
 */
static inline void bucketize_local_keys(
  const std::vector<KEY_TYPE> &my_keys,
  dash::NArray<int, 3>        &buckets,
  dash::NArray<int, 2>        &bucket_sizes);

/*
 * Each PE sends the contents of its local buckets to the PE that owns that bucket.
 */
static inline std::vector<KEY_TYPE> exchange_keys(
  dash::NArray<int, 3> &buckets,
  dash::NArray<int, 2> &bucket_sizes,
  long long int &my_bucket_size);

/*
 * Count the occurence of each key within my bucket. 
 */
static inline std::vector<int>
count_local_keys(const std::vector<KEY_TYPE>& my_bucket_keys,
                 const long long int my_bucket_size);

/*
 * Verifies the correctness of the sort. 
 * Ensures all keys after the exchange are within a PE's bucket boundaries.
 * Ensures the final number of keys is equal to the initial.
 */
static int verify_results(std::vector<int>      &my_local_key_counts,
                          std::vector<KEY_TYPE> &my_local_keys,
                          const long long int my_bucket_size);

/*
 * Seeds each rank based on the rank number and time
 */
static inline pcg32_random_t seed_my_rank(void);

static int file_exists(char * filename);

static void log_times(char * log_directory);
static void report_summary_stats(void);
static void print_timer_names(FILE * fp);
static void print_run_info(FILE * fp);
static void print_timer_values(FILE * fp);
static std::vector<double> gather_rank_times(_timer_t * const timer);
static std::vector<unsigned int> gather_rank_counts(_timer_t * const timer);

#endif
