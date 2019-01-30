# ifndef AGGREGATION_HPP
# define AGGREGATION_HPP

# include<stdlib.h>
# include<cycles/cycles.hpp>
# include<circuits/aggregator_circuit.hpp>

template<typename cycle, std::size_t FromN, std::size_t arityN>
class aggregation {

private:

    aggregation(){}
    static aggregation instance;

public:

    /**
     * @brief 
     * 
     */
    using cycleT        = cycle;
    using fromT         = typename cycle::template get_curve<FromN>::type;
    using toT           = typename cycle::template get_curve<(FromN+1)%cycleT::size>::type;

    using from_fieldT   = typename fromT::Fp_type;
    using to_fieldT     = typename toT::Fp_type;

    /**
     * @brief 
     * 
     */
    static constexpr std::size_t fromN = FromN;
    static constexpr std::size_t arity = arityN;

    std::string name;
    std::string dir;

    std::shared_ptr<aggregator_circuit<fromT, toT>>         circuit;
    r1cs_constraint_system<to_fieldT>                       constraint_system;
    std::shared_ptr<r1cs_ppzksnark_keypair<toT>>            keypair;

    aggregation& with_circuit();
    aggregation& with_name(std::string _name);
    aggregation& at_dir(std::string dir);
    aggregation& with_crs();

    static aggregation& get_instance(){ return instance; }

    libsnark::r1cs_ppzksnark_primary_input<toT> aggregate_inputs(
        std::vector<libsnark::r1cs_ppzksnark_primary_input<fromT>> &primary_inputs,
        std::vector<libsnark::r1cs_ppzksnark_verification_key<fromT>> &verification_keys,
        std::vector<libsnark::r1cs_ppzksnark_proof<fromT>> &proofs 
    );

    libsnark::r1cs_ppzksnark_proof<toT> aggregate_proofs(
        std::vector<libsnark::r1cs_ppzksnark_primary_input<fromT>> &primary_inputs,
        std::vector<libsnark::r1cs_ppzksnark_verification_key<fromT>> &verification_keys,
        std::vector<libsnark::r1cs_ppzksnark_proof<fromT>> &proofs 
    );

private:

    bool initialized;
    bool setup_done();

    void load_crs();
    void save_crs();
    void generator();

};

# include <cycles/aggregation.tcc>

# endif