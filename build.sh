#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

eosiocpp -o eosotc.wast eosotc.cpp &  eosiocpp -g eosotc.abi eosotc.cpp 

cleos  set contract eosotc ../eosotc   -p eosotc@active

#cleos  transfer user1  eosotc  '1 DDD' 'opt=1'

#cleos  transfer user1  eosotc  '1 DDD' 'order_id=999&opt=2&amount=10&token_id=12345'

#cleos get table eosotc eosotc orders
#cleos get table eosotc eosotc markets