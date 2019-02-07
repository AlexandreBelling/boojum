# ifndef AGGREGATION_TCC
# define AGGREGATION_TCC

# include <cycles/aggregation.hpp>
# include <materials/file.hpp>
# include <stdlib.h>

using namespace std;
using namespace libsnark;

template<typename cycle, std::size_t fromN, std::size_t arityN>
aggregation<cycle, fromN, arityN> aggregation<cycle, fromN, arityN>::instance;

template<typename cycle, std::size_t fromN, std::size_t arityN>
aggregation<cycle, fromN, arityN>& aggregation<cycle, fromN, arityN>::with_circuit()
{
    circuit.reset(new aggregator_circuit<fromT, toT>(arityN));
    circuit->generate_r1cs_constraints();
    constraint_system = circuit->pb.get_constraint_system();
    return *this;
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
aggregation<cycle, fromN, arityN>& aggregation<cycle, fromN, arityN>::with_name(string _name)
{
    name = _name;
    return *this;
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
aggregation<cycle, fromN, arityN>& aggregation<cycle, fromN, arityN>::at_dir(string _dir)
{
    dir = _dir;
    return *this;
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
aggregation<cycle, fromN, arityN>& aggregation<cycle, fromN, arityN>::with_crs()
{

    if (setup_done())
    {
        load_crs();
    } else {

        generator();
        save_crs();
        
    }

    return *this;
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
    auto aggregation<cycle, fromN, arityN>::aggregate_proofs
(
    std::vector<libsnark::r1cs_ppzksnark_primary_input<fromT>>      &primary_inputs,
    std::vector<libsnark::r1cs_ppzksnark_verification_key<fromT>>   &verification_keys,
    std::vector<libsnark::r1cs_ppzksnark_proof<fromT>>              &proofs
) -> r1cs_ppzksnark_proof<toT>
{

    circuit->generate_r1cs_witness(
        verification_keys,
        primary_inputs,
        proofs
    );

    return r1cs_ppzksnark_prover<toT>(
        keypair->pk,
        circuit->pb.primary_input(),
        circuit->pb.auxiliary_input()
    );
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
    auto aggregation<cycle, fromN, arityN>::aggregate_inputs
(
    std::vector<libsnark::r1cs_ppzksnark_primary_input<fromT>>      &primary_inputs,
    std::vector<libsnark::r1cs_ppzksnark_verification_key<fromT>>   &verification_keys,
    std::vector<libsnark::r1cs_ppzksnark_proof<fromT>>              &proofs 

) -> r1cs_ppzksnark_primary_input<toT>
{

    circuit->generate_primary_inputs(
        verification_keys,
        primary_inputs,
        proofs
    );

    return circuit->pb.primary_input();    
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
bool aggregation<cycle, fromN, arityN>::setup_done(){
        bool res = file<int>::is_file(dir, name + ".pk");
        res &= file<int>::is_file(dir, name + ".vk");
        return res;
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
void aggregation<cycle, fromN, arityN>::load_crs()
{
    keypair.reset(new r1cs_ppzksnark_keypair<toT>());
    file<r1cs_ppzksnark_proving_key<toT>>::load_from_file(
        keypair->pk,
        dir,
        name + ".pk"
    );
    file<r1cs_ppzksnark_verification_key<toT>>::load_from_file(
        keypair->vk,
        dir,
        name + ".vk"
    );
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
void aggregation<cycle, fromN, arityN>::save_crs()
{
    file<r1cs_ppzksnark_proving_key<toT>>::save_to_file(
        keypair->pk,
        dir,
        name + ".pk"
    );
    file<r1cs_ppzksnark_verification_key<toT>>::save_to_file(
        keypair->vk,
        dir,
        name + ".vk"
    );
}

template<typename cycle, std::size_t fromN, std::size_t arityN>
void aggregation<cycle, fromN, arityN>::generator()
{
    auto _keypair = r1cs_ppzksnark_generator<toT>(constraint_system);
    keypair = make_shared<decltype(_keypair)>(_keypair);
}

# endif