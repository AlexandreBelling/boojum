# ifndef SNARK_COMPILE
# define SNARk_COMPILE

# include <libff/common/profiling.hpp>
# include <cycles/basic_aggregation_system.tcc>
# include <trees/tree.hpp>
# include <materials/trees.hpp>
# include <iostream>
# include <libboojum.h>

void initialize() 
{

    // Decreases libsnark verbosity
    libff::inhibit_profiling_info = true;
    libff::inhibit_profiling_counters = true;

    dual_basT::initialize_curves();
    std::cout << "Done all curves" << std::endl;

    dual_basT::build_circuits();
    std::cout << "Done all circuits" << std::endl;

}

// Initialize should be run as a prerequisite of generation no matter what
void run_generators(const char* dir) 
{
    auto& forward = forwardT::get_instance();
    auto& backward = backwardT::get_instance();

    string dir_str(dir);

    // Run generator or Load generated
    forward.with_name("forward").at_dir(dir).with_crs();
    backward.with_name("backward").at_dir(dir).with_crs();

};

void make_example_proof(
    void** tree_buffer
) {
    unsigned char ** tree_buffer_char = reinterpret_cast<unsigned char **>(tree_buffer);
    //memfreetree(*tree_buffer_char);
    auto new_value = tree<dual_basT, 0>::as_example(50).to_string();
    tree_buffer_char[0] = new_value;

    for(int q=0; q<16; q++)
    {
        printf("%x %x %x\n", tree_buffer, tree_buffer_char[0], tree_buffer_char[0][q]);
    }

    std::vector<std::uint32_t> header = parse_header(*tree_buffer_char);
}

template<std::size_t I>
void aggregate_steps(
    unsigned char * left_node_char,
    unsigned char * right_node_char, 
    unsigned char ** output_node_char
)
{
    std::vector<tree<dual_basT, I>> children(2);
    children[0] = tree<dual_basT, I>::from_string(left_node_char);
    children[1] = tree<dual_basT, I>::from_string(right_node_char);

    auto aggregated_tree = tree<dual_basT, (I+1)%2>::from_aggregation(children);
    aggregated_tree.aggregate_inputs().aggregate_proofs();
    *output_node_char = aggregated_tree.to_string();

    return;
}

void prove_aggregation(
    void * left_node_buff,
    void * right_node_buff, 
    void ** output_node_buff
) {

    unsigned char *left_node_char = reinterpret_cast<unsigned char *>(left_node_buff);
    unsigned char *right_node_char = reinterpret_cast<unsigned char *>(right_node_buff);
    unsigned char **output_node_char = reinterpret_cast<unsigned char **>(output_node_buff);

    //memfreetree(*output_node_char);

    std::vector<std::uint32_t> l_header = parse_header(left_node_char);
    std::vector<std::uint32_t> r_header = parse_header(right_node_char);

    if(l_header[2] != r_header[2])
    {
        std::cout << "Input nodes do not have the same height" << std::endl;
        return;
    }

    if(l_header[3] != r_header[3])
    {
        std::cout << "Input trees do not have the same arities" << std::endl;
        return;
    }

    switch(l_header[1]){
        case 0: 
            break;

        default:
            std::cout << "Only zeroth type is supported yet" << std::endl;
            return;
    }

    switch(l_header[3]){
        case 2: 
            break;

        default:
            std::cout << "Only arity 2 is supported yet" << std::endl;
            return;
    }

    switch(l_header[2]){
        case 0:
            aggregate_steps<0>(left_node_char, right_node_char, output_node_char);
            return;
        case 1:
            aggregate_steps<1>(left_node_char, right_node_char, output_node_char);
            return;
        default:
            std::cout << "Only arity two is supported" << std::endl;
            return;
    }
}

template<std::size_t I>
bool verify(unsigned char * node_char)
{
    auto aggregated_tree = tree<dual_basT, I>::from_string(node_char);
    return aggregated_tree.aggregate_inputs().verify();
}

bool verify(
    void* node_buff
) {
    unsigned char *node_char = reinterpret_cast<unsigned char *>(node_buff);
    std::vector<std::uint32_t> header = parse_header(node_char);

    switch(header[1]){
        case 0: 
            break;

        default:
            std::cout << "Only zeroth type is supported yet" << std::endl;
            return false;
    }

    switch(header[3]){
        case 2: 
            break;

        default:
            std::cout << "Only arity 2 is supported yet" << std::endl;
            return false;
    }

    switch(header[2])
    {
        case 0:
            return verify<0>(node_char);
        case 1:
            return verify<1>(node_char);
        default:
            std::cout << "Only arity two is supported" << std::endl;
            return false;
    }

    return false;
}

void memfree(
    void* node_buff
) {
    unsigned char *node_char = reinterpret_cast<unsigned char *>(node_buff);
    memfreetree(*node_char);
}

#endif