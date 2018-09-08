#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

eosiocpp -o eosotc.wast eosotc.cpp &  eosiocpp -g eosotcabi eosotc.cpp 

cleos  set contract tester ../eosotc   -p tester@active

cleos  transfer user  tester  '1 EOS' 'order_id=999&opt=1&amount=10&&token_id=12345'

#cleos get table tester tester order