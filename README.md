The following instructions assume that the Web IDE was started successfully (see [Setup](#setup)).

## Opening a terminal

To open a terminal, use the Terminal drop-down menu in the IDE user interface.

## Building sample contract

The source code for the sample smartcontract is at `contract/accounts.cpp` within the IDE. To compile the contract, run this in a terminal:

```
## Compile
eosio-cpp contract/acckata1.cpp  -I=eosio.contracts/contracts/eosio.token/include/
cleos set contract acckata1 $(pwd) --abi acckata1.abi -p acckata1@active

## Create accounts
cleos create account eosio acckata1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission acckata1 active --add-code

cleos create account eosio skoyya EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set account permission skoyya active --add-code

##Create Wallet
4 cleos wallet open
unlock the walet as below and give password

5 cleos wallet unlock
Password for Wallet , PW5JBTprqRqbQVUX3nFXtqmeECJKiSTXqYGqcsC8o7YHurNsw1Adf

6 cleos wallet create_key
public key EOS5Y5y7MadPZGz4GWP9k6QaBhCptSuBfzKnqnbGrY7nen3HZhxyh

Show All 
cleos -v push action acckata1   showall '[ "" ]' -p acckata1@active

create account  
cleos push action acckata1  createusr '["skoyya"]' -p skoyya@active
cleos push action acckata1  createusr '["acckata1"]' -p acckata1@active

depost
cleos push action acckata1  deposit '["skoyya", "default" 500]' -p skoyya@active
cleos push action acckata1  deposit '["acckata1", "default", 700]' -p acckata1@active
cleos push action acckata1  deposit '["acckata1", "check", 200]' -p acckata1@active

transferctg
cleos push action acckata1  transferctg '["skoyya", "default", "check", 300]' -p skoyya@active

transferusr
cleos push action acckata1  transferusr '["skoyya", "acckata1", "check", "savng",  100]' -p skoyya@active

