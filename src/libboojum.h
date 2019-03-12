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

    void initialize();

    void run_generators(const char* dir);

    void make_example_proof(
        void** tree_buffer
    );

    void prove_aggregation(
        void * left_node_buff,
        void * right_node_buff, 
        void ** output_node_buff
    );

    bool verify(
        void* node_buff
    );

    void memfree(
        void* node_buff
    );

#ifdef __cplusplus
}
#endif

#endif