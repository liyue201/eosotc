#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

cleos create account eosio user1 EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio user2 EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio eosotc EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio eosio.token EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos set contract eosio.token /contracts/eosio.token -p eosio.token

cleos push action eosio.token create '{"issuer":"eosio.token", "maximum_supply":"1000000000.0000 EOS", "can_freeze":0, "can_recall":0, "can_whitelist":0}' -p eosio.token

cleos push action eosio.token create '{"issuer":"eosio.token", "maximum_supply":"1000000000.0000 DDD", "can_freeze":0, "can_recall":0, "can_whitelist":0}' -p eosio.token


cleos push action eosio.token issue '[ "user1", "100000.0000 EOS", "memo" ]' -p eosio.token
cleos push action eosio.token issue '[ "user1", "100000.0000 DDD", "memo" ]' -p eosio.token

#查看代币余额
cleos get currency balance eosio.token  user1
cleos get currency balance eosio.token  user2

#转账
cleos  transfer user1  user2  '10000 EOS' 'memo'
cleos  transfer user1  user2  '10000 DDD' 'memo'