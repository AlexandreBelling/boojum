# ifndef TOOLS_TCC
# define TOOLS_TCC

# include "tools.hpp"

using namespace libsnark;
using namespace std;

template<typename curve, typename other_curve>
tools<curve, other_curve> tools<curve, other_curve>::instance;

template<typename curve, typename other_curve>
void tools<curve, other_curve>::initialize(string name_) {

    if (initialized) {
         return ;
    }
    name = name_;

    // Load the circuits for A -> B
    circuit.reset(new aggregator_circuit<curve, other_curve>(2));
    circuit->generate_r1cs_constraints();
    constraint_system = circuit->pb.get_constraint_system();
}

template<typename curve, typename other_curve>
void tools<curve, other_curve>::fetch_crs(
    string path, 
    bool overwrite
) {
    path_str = path;

    // Initialize the path of the keypairs 
    pk_name = path_str + "/" + name + ".pk";
    vk_name = path_str + "/" + name + ".vk";

    if (setup_done() || overwrite) {
        load_key_pair();
    } else {
        // Run generators
        key_pair = make_shared<r1cs_ppzksnark_keypair<curve>>(
            r1cs_ppzksnark_generator<curve>(constraint_system)
        );
        save_key_pair();
    }
}

template<typename curve, typename other_curve>
void tools<curve, other_curve>::load_key_pair(){
    key_pair.reset(
        new r1cs_ppzksnark_keypair<curve>()
        );
    //Loading keypairs
    file::load_from_file<r1cs_ppzksnark_proving_key<curve>>(
        pk_name,
        key_pair->pk
        );
    file::load_from_file<r1cs_ppzksnark_verification_key<curve>>(
        vk_name,
        key_pair->vk
        );
}

template<typename curve, typename other_curve>
void tools<curve, other_curve>::save_key_pair(){
    file::save_to_file<r1cs_ppzksnark_proving_key<curve>>(
        pk_name,
        key_pair->pk
        );
    file::save_to_file<r1cs_ppzksnark_verification_key<curve>>(
        vk_name,
        key_pair->vk
        );
}

# endif