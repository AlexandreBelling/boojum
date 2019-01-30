# ifndef TOOLS_HPP
# define TOOLS_HPP

# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
# include "serialize.hpp"
# include "circuits/aggregator_circuit.hpp"
# include "libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp"
# include "libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp"

template<typename curve, typename other_curve>
class tools {
private: 
    tools(){}
public:

    using field = libff::Fr<curve>;
    using other_field = libff::Fr<other_curve>;

    // Tools is a singleton
    static tools instance;

    tools(tools const&) = delete;
    void operator=(tools const&) = delete;
    static tools& get_instance(){ return instance; }

    // Path to where the keypairs are stored
    std::string path_str;
    std::string name;
    std::string pk_name;
    std::string vk_name;

    // Circuits related variables for A -> B
    std::shared_ptr<aggregator_circuit<curve, other_curve>> circuit;
    r1cs_constraint_system<field> constraint_system;
    std::shared_ptr<r1cs_ppzksnark_keypair<curve>> key_pair;

    // Size of a serialized proof
    // A :
    // B :
    int node_proof_size;
    // B : 1315
    // A : 1075
    int node_proof_size_leaf;

    // It can be initialized only once
    bool initialized;

    // generate the circuits
    void initialize(std::string name);

    // Runs the setup if not done already
    void fetch_crs(
        std::string path="./setup", 
        bool overwrite=false
    );

private:

    bool setup_done() {
        bool res = file::is_file(pk_name);
        res &= file::is_file(vk_name);
        return res;
    }

    void load_key_pair();
    void save_key_pair();
};

#include "tools.tcc"

# endif