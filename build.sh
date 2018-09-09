#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

eosiocpp -o eosotc.wast eosotc.cpp &  eosiocpp -g eosotc.abi eosotc.cpp 

cleos  set contract eosotc ../eosotc   -p eosotc@active

#cleos  transfer user1  eosotc  '1 DDD' 'opt=1'

#cleos  transfer user1  eosotc  '2 EOS' 'opt=4&amount=10000&token_contract=6138663591592764928&token_symbol=1145324548'
#cleos  transfer user1  eosotc  '5 DDD' 'opt=4&amount=10000'

#cleos get table eosotc eosotc orders
#cleos get table eosotc eosotc markets