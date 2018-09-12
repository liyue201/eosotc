#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

eosiocpp -o eosotc.wast eosotc.cpp &  eosiocpp -g eosotc.abi eosotc.cpp 

cleos  set contract ddddcontract ../eosotc   -p ddddcontract@active
