#!/bin/bash

shopt -s expand_aliases

alias cltest='cleos -u  http://kylin.fn.eosbixin.com'

#alias cltest='cleos -u  http://api-kylin.eosasia.one'

#alias cltest='cleos -u  http://api.kylin.alohaeos.com'

cltest wallet unlock -n testnet --password PW5JGEZzfMX85xCi36UFrvbSCPDKWRNGhgZv4XsnbmyoB9omeAbKs

cltest set contract ddddcontract ../eosotc   -p ddddcontract@active

cltest set account permission ddddcontract active '{"threshold": 1,"keys": [{"key": "EOS7JhDD7A2pTVg5pSzgXZKc4czL6Aza1JzuzG8CHC8vpPD8Xwo5N","weight": 1}],"accounts": [{"permission":{"actor":"ddddcontract","permission":"eosio.code"},"weight":1}]}' owner -p ddddcontract