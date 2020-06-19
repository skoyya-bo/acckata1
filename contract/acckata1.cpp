#include <eosio/eosio.hpp>
#include <eosio.token/eosio.token.hpp>

using namespace eosio;

class acckata1 : eosio::contract {
    struct [[eosio::table]] account_balance
    {
	    name user;
        std::map<std::string, double> type_balance;
        uint64_t primary_key() const { return user.value; }
    };

    using acc_table = eosio::multi_index<"user.ind"_n, account_balance>;

    constexpr static char default_category[] = "default";

    double get_default_balance(name nm) { 
        return eosio::token::get_balance(eosio::name("eosio.token"), nm, eosio::symbol_code("SYS")).amount;
    }

  public:
    // Use contract's constructor
    using contract::contract;

    // Deposit into an existing account
    [[eosio::action]] void deposit(name user, std::string type, double amount) {
        // Check user
        require_auth(user);
	    check(amount >= 0, "Invalid amount to deposit");
        acc_table accounts{get_self(), 0};
	    auto account = accounts.get(user.value);//throws exception if account doesnt exists for user
	    account.type_balance[type]  += amount;
	
	    //update
	    accounts.modify(accounts.find(user.value), user, [&](auto& acc) {
            acc.user         = account.user;
		    acc.type_balance = account.type_balance;
        });
    }

    // Create an account
    [[eosio::action]] void createusr(name user) {
        // Check user
	    require_auth(user);
        acc_table accounts{get_self(), 0};

	    accounts.emplace(get_self(), [&](auto& acc) {
            acc.user         = user;
            acc.type_balance[default_category] = 0.0;//get_default_balance(user);
	    });
    }

    //Delete user
    [[eosio::action]] void deleteusr(name user) {
        // Check user
	    require_auth(user);
        acc_table accounts{get_self(), 0};
        auto account = accounts.get(user.value);//throws exception if account doesnt exists for user
        accounts.erase(accounts.find(user.value));
    }

    //create category
    [[eosio::action]] void addctgry(name user, std::string ctgry, double balance) {
        check( ctgry != default_category, "Can not create default category");
        //check(get_current_balabce(user) <= balance, "Insufficient balance");
	    require_auth(user);
        acc_table accounts{get_self(), 0};
	    auto account = accounts.get(user.value);
        account.type_balance[ctgry] = balance;
	    accounts.modify(accounts.find(user.value), user, [&](auto& acc) {
            acc.user         = account.user;
		    acc.type_balance = account.type_balance;
        });
    }



    // Transfer amount between categories
    [[eosio::action]] void transferctg(name user, std::string fromcat, std::string tocat, double trnsfamnt ) {
        // Check user
	    require_auth(user);
	    acc_table accounts{get_self(), 0};
	    auto account = accounts.get(user.value);
	    auto it = account.type_balance.find(fromcat);
	    check(it != std::end(account.type_balance), "Account type "+fromcat+" doesnt exists");
	    check(it->second >=trnsfamnt , "Insufficient balance in acc type: "+ fromcat);
        auto type_balance = account.type_balance;
	    type_balance[fromcat] = it->second - trnsfamnt;
	    type_balance[tocat] += trnsfamnt;

	    //update
	    accounts.modify(accounts.find(user.value), user, [&](auto& acc) {
            acc.user         = user;
		    acc.type_balance = type_balance;
        });
    }

  //Transfer amount accross accounts
  [[eosio::action]] void transferusr(name fromuser, name touser, std::string fromacctype, std::string toacctype, double trnsfamnt) {
    	require_auth(fromuser);

	    acc_table accounts{get_self(), 0};
	    auto from_account = accounts.get(fromuser.value);
	    auto to_account = accounts.get(touser.value);
	    auto from_it = from_account.type_balance.find(fromacctype);
	    auto to_it = to_account.type_balance.find(toacctype);
	    check(from_it != std::end(from_account.type_balance), "Account type "+fromacctype+" doesnt exists");
	    //check(to_it != std::end(to_account.type_balance), "Account type "+toacctype+" doesnt exists");

	    auto from_acc_balance = from_it->second;
	    auto to_acc_balance = to_it->second;
	    check(from_acc_balance >= trnsfamnt, "Insufficient balance in from user["+fromuser.to_string()+"] acc type["+fromacctype+"]");	

	    from_acc_balance -= trnsfamnt;
	    to_acc_balance += trnsfamnt;
        auto from_type_balance = from_account.type_balance;
        auto to_type_balance = to_account.type_balance;
        from_type_balance[fromacctype] = from_acc_balance;
        to_type_balance[toacctype] = to_acc_balance;


	    //update
	    accounts.modify(accounts.find(fromuser.value), fromuser, [&](auto& acc) {
            acc.user         = from_account.user;
		    acc.type_balance = from_type_balance;
        });
	    accounts.modify(accounts.find(touser.value), touser, [&](auto& acc) {
            acc.user         = to_account.user;
		    acc.type_balance = to_type_balance;
        });
   }

   //Show all users and their accounts and balance information
   [[eosio::action]] void showall() {
        require_auth(get_self());
	    acc_table accounts{get_self(), 0};
	    for( auto acc=std::begin(accounts); acc != std::end(accounts); ++acc) {
	        for(auto it2=std::begin(acc->type_balance); it2 != std::end(acc->type_balance); ++it2) {
		        print(acc->user.to_string(), " -- ", it2->first, " -- " , std::to_string(it2->second), "\n");
		    }
	    }
   }

   private:
    
    double get_current_balabce(name nm) {
        double token_amt = get_default_balance(nm);
        acc_table accounts{get_self(), 0};

        auto account = accounts.get(nm.value);
        double curr_balance =0.0;
        for( auto it = std::begin(account.type_balance); it != std::end(account.type_balance); ++it ) {
            if ( it->first != default_category)
                curr_balance += it->second;
        }
        return token_amt - curr_balance;
    }
};
