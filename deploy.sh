#!/bin/bash

shopt -s expand_aliases

#alias cltest='cleos -u  http://kylin.fn.eosbixin.com'

alias cltest='cleos -u  http://api-kylin.eosasia.one'

#alias cltest='cleos -u  http://api.kylin.alohaeos.com'

cltest wallet unlock -n testnet --password PW5JGEZzfMX85xCi36UFrvbSCPDKWRNGhgZv4XsnbmyoB9omeAbKs

cltest  set contract bbbbbbbbnnnn ../eosotc   -p bbbbbbbbnnnn@active