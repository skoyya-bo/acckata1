#include <boost/test/unit_test.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/permission_object.hpp>
#include <eosio/chain/resource_limits.hpp>
#include <eosio/testing/tester.hpp>

using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;

BOOST_AUTO_TEST_SUITE(acckata1_tests)

BOOST_AUTO_TEST_CASE(post) try {
    tester t{setup_policy::none};

    // Load contract
    t.create_account(N(acckata1));
    t.set_code(N(acckata1), read_wasm("acckata1.wasm"));
    t.set_abi(N(acckata1), read_abi("acckata1.abi").data());
    t.produce_block();

    // Create users
    t.create_account(N(john));
    t.create_account(N(jane));

    // Test "createusr" action -- happy case
    t.push_action(
        N(acckata1), N(createusr), N(john),
        mutable_variant_object //
        ("user", "john")    //
    );
    
    //will be used later so creating an account
    t.push_action(
        N(acckata1), N(createusr), N(jane),
        mutable_variant_object //
        ("user", "jane")    //
    );

    // Test "createusr" action -- duplicate user throws exception
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(acckata1), N(createusr), N(john),
                mutable_variant_object       //
                ("user", "john")             //
            );
        }(),
        fc::exception);

    // Test "deposit" action -- happy case
    t.push_action(
        N(acckata1), N(deposit), N(john),
        mutable_variant_object //
        ("user", "john")    //
        ("type", "default")    //
        ("amount",200)     //
    );

    // Test "deposit" action -- deposit into non existing acc. throws exception
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(acckata1), N(deposit), N(john),
                mutable_variant_object //
                ("user", "dummy")    //
                ("type", "chck")    //
                ("amount",200)     //
            );
        }(),
        fc::exception);

    // Test "addctgry" action - happy case 
    t.push_action(
        N(acckata1), N(addctgry), N(john),
        mutable_variant_object //
        ("user", "john")    //
        ("ctgry", "svng")    //
        ("balance", 200)     //
    );

    // Test "addctgry" action - add category into non-existing acc. Throws exception 
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
                N(acckata1), N(addctgry), N(john),
                mutable_variant_object //
                ("user", "dummy")    //
                ("ctgry", "svng")    //
                ("balance", 200)     //
            );
        }(),
        fc::exception);
    
    //Test - "transferctg" - transfer between categories within acc. -- happy case 
    t.push_action(
        N(acckata1), N(transferctg), N(john),
        mutable_variant_object //
        ("user", "john")    //
        ("fromcat", "default")    //
        ("tocat", "spcl")   //
        ("trnsfamnt", 1)     //
    );
//Test - "transferctg" - transfer in an invalid account. -- throws exception
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
            N(acckata1), N(transferctg), N(john),
            mutable_variant_object //
            ("user", "dummy")    //
            ("fromcat", "default")    //
            ("tocat", "spcl")   //
            ("trnsfamnt", 1)     //
            );
        }(),
        fc::exception);

    //Test - "transferctg" - Invalid source category. -- throws exception
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
            N(acckata1), N(transferctg), N(john),
            mutable_variant_object //
            ("user", "john")    //
            ("fromcat", "dummy")    //
            ("tocat", "spcl")   //
            ("trnsfamnt", 1)     //
            );
        }(),
        fc::exception);

//Test - "transferctg" - Insufficient balance. -- throws exception
    BOOST_CHECK_THROW(
        [&] {
            t.push_action(
            N(acckata1), N(transferctg), N(john),
            mutable_variant_object  //
            ("user", "john")        //
            ("fromcat", "default")  //
            ("tocat", "spcl")       //
            ("trnsfamnt", 3000)     //
            );
        }(),
        fc::exception);
}

FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
