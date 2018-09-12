#!/bin/bash

shopt -s expand_aliases

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

cleos create account eosio dddddddadmin EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio ddddcontract EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio ddddddduser1 EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio ddddddduser2 EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos create account eosio eosio.token EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G

cleos set contract eosio.token /contracts/eosio.token -p eosio.token

cleos push action eosio.token create '{"issuer":"eosio.token", "maximum_supply":"1000000000.0000 EOS", "can_freeze":0, "can_recall":0, "can_whitelist":0}' -p eosio.token

cleos push action eosio.token issue '[ "ddddddduser1", "100000.0000 EOS", "memo" ]' -p eosio.token

cleos set contract dddddddadmin /contracts/eosio.token -p dddddddadmin
cleos push action dddddddadmin create '{"issuer":"dddddddadmin", "maximum_supply":"1000000000.0000 DDD", "can_freeze":0, "can_recall":0, "can_whitelist":0}' -p dddddddadmin
cleos push action dddddddadmin issue '[ "ddddddduser1", "100000.0000 DDD", "memo" ]' -p dddddddadmin

#查看代币余额

#转账
cleos  transfer ddddddduser1  ddddddduser2  '10000 EOS' 'memo'
cleos  transfer ddddddduser1  dddddddadmin  '10000 EOS' 'memo'
cleos push action dddddddadmin transfer '["ddddddduser1", "ddddddduser2", "1000.0000 DDD", "memo" ]' -p ddddddduser1
cleos push action dddddddadmin transfer '["ddddddduser1", "dddddddadmin", "1000.0000 DDD", "memo" ]' -p ddddddduser1

#查看代币余额
cleos get currency balance eosio.token  ddddddduser1
cleos get currency balance eosio.token  ddddddduser2
cleos get currency balance eosio.token   dddddddadmin
cleos get currency balance dddddddadmin  ddddddduser1
cleos get currency balance dddddddadmin  ddddddduser2
cleos get currency balance dddddddadmin  dddddddadmin

#增加权限
cleos set account permission ddddcontract active '{"threshold": 1,"keys": [{"key": "EOS5oR8TdbycNttjL4hYe5KSwGuWVj1YGy8H2gvKHv6J6nTU5yt7G","weight": 1}],"accounts": [{"permission":{"actor":"ddddcontract","permission":"eosio.code"},"weight":1}]}' owner -p ddddcontract