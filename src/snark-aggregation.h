#ifndef SNARK_IMPL_H_
#define SNARK_IMPL_H_

#include <stdlib.h>

#ifndef __cplusplus
#ifndef bool
#include <stdbool.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void initialize(const char* path);

    void run_generators(const char* path);

    void make_example_proof(
        void* proof_buff,
        void* verification_key_buff,
        void* primary_input_buff
    );

    void prove_aggregation(
        bool is_ab,
        void* output_node_buff,
        void* left_node_buff,
        void* right_node_buff
    );

    bool verify(
        bool is_ab,
        void* node_buff
    );

#ifdef __cplusplus
}
#endif

#endif