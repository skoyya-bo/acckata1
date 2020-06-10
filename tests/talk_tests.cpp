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

    // Test "create" action
    t.push_action(
        N(acckata1), N(create), N(john),
        mutable_variant_object //
        ("user", N(john))              //
        ("defamount", 100)              //
    );
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
