#include <eosio/eosio.hpp>
#include <eosio.token/eosio.token.hpp>
//#include <eosio/asset.hpp>

using namespace eosio;

class acckata1 : eosio::contract {
    struct [[eosio::table]] account_balance
    {
	name user;
        std::map<uint64_t, asset> type_balance;
        uint64_t primary_key() const { return user.value; }
    };

    using acc_table = eosio::multi_index<"user.ind"_n, account_balance>;

  public:
    // Use contract's constructor
    using contract::contract;

    // Deposit into an existing account
    [[eosio::action]] void deposit(name user, asset amount) {


        // Check user
        require_auth(user);

	check(amount <= asset(0, amount.symbol), "Invalid amount to deposit, "+amount.to_string());

        acc_table accounts{get_self(), 0};

	auto account = accounts.get(user.value);//throws exception if account doesnt exists for user
	account.type_balance[amount.symbol.raw()]  += amount;
	
	//update
	accounts.erase(account);
	accounts.emplace(get_self(), [&](auto& acc) {
                acc.user         = account.user;
		acc.type_balance = account.type_balance;
            });
    }

    // Create an account
    [[eosio::action]] void create(name user, name acc_type, asset def_amount) {
        // Check user
	require_auth(user);

	acc_table accounts{get_self(), 0};
	accounts.emplace(get_self(), [&](auto& acc) {
            acc.user         = user;
            acc.type_balance[def_amount.symbol.raw()] = def_amount;
	});
    }

    // Transfer amount between categories/symbols
    [[eosio::action]] void transfer(name user, std::string from_acc_typ, std::string to_acc_typ, double trnsf_amnt ) {
        // Check user
	require_auth(user);

	acc_table accounts{get_self(), 0};
	auto account = accounts.get(user.value);
	auto it = account.type_balance.find(symbol(from_acc_typ, 4).raw());
	check(it != std::end(account.type_balance), "Account type "+from_acc_typ+" doesnt exists");
	auto acc_balance = it->second;
	check(acc_balance.amount >=trnsf_amnt , "Insufficient balance in acc type: "+ from_acc_typ);
	acc_balance.amount -= trnsf_amnt;
	account.type_balance[symbol(from_acc_typ, 4).raw()] = acc_balance;
	account.type_balance[symbol(to_acc_typ, 4).raw()] += asset(trnsf_amnt, symbol(to_acc_typ, 4));

	//update
	accounts.erase(account);
	accounts.emplace(get_self(), [&](auto& acc) {
                acc.user         = account.user;
		acc.type_balance = account.type_balance;
            });
    }

  //Transfer amount accross accounts
  [[eosio::action]] void transfer2(name from_user, name to_user, std::string from_acc_typ, std::string to_acc_typ, double trnsf_amnt) {
	require_auth(from_user);

	acc_table accounts{get_self(), 0};
	auto from_account = accounts.get(from_user.value);
	auto to_account = accounts.get(to_user.value);
	auto from_it = from_account.type_balance.find(symbol(from_acc_typ, 4).raw());
	auto to_it = to_account.type_balance.find(symbol(to_acc_typ, 4).raw());
	check(from_it != std::end(from_account.type_balance), "Account type "+from_acc_typ+" doesnt exists");
	check(to_it != std::end(to_account.type_balance), "Account type "+to_acc_typ+" doesnt exists");

	auto from_acc_balance = from_it->second;
	auto to_acc_balance = to_it->second;
	check(from_acc_balance.amount >= trnsf_amnt, "Insufficient balance in from user["+from_user.to_string()+"] acc_type["+from_acc_typ+"]");	

	from_acc_balance.amount -= trnsf_amnt;
	to_acc_balance.amount += trnsf_amnt;

	//update
	accounts.erase(from_account);
	accounts.emplace(get_self(), [&](auto& acc) {
                acc.user         = from_account.user;
		acc.type_balance = from_account.type_balance;
            });
	accounts.erase(to_account);
	accounts.emplace(get_self(), [&](auto& acc) {
                acc.user         = to_account.user;
		acc.type_balance = to_account.type_balance;
            });
   }

   //Show all users and their accounts and balance information
   [[eosio::action]] void showall() {
        require_auth(get_self());
	acc_table accounts{get_self(), 0};
	for( auto acc=std::begin(accounts); acc != std::end(accounts); ++acc) {
	    for(auto it2=std::begin(acc->type_balance); it2 != std::end(acc->type_balance); ++it2) {
		print(acc->user.to_string(), " -- ", it2->second.to_string(), "\n");
		}
	}
   }
};
