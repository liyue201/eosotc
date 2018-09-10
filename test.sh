#!/bin/bash

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

#创建市场
#cleos  transfer user1  eosotc  '1 DDD' 'opt=1'

#打开市场
#cleos  transfer user1  eosotc  '1 DDD' 'opt=2'

#挂单：用2个EOS买1个DDD 
#cleos  transfer user1  eosotc  '2 EOS' 'opt=4&amount=10000&token_contract=6138663591592764928&token_symbol=1145324548'

#挂单：卖5个DDD，价格是1个EOS
#cleos  transfer user1  eosotc  '5 DDD' 'opt=4&amount=10000'

#吃单
#cleos  transfer user2  eosotc  '1 EOS' 'opt=5&order_id=0&token_contract=6138663591592764928&token_symbol=1145324548'

#cleos  transfer user2  eosotc  '1 DDD' 'opt=5&order_id=0'

#cleos get table eosotc eosotc askorders
#cleos get table eosotc eosotc bidorders
#cleos get table eosotc eosotc markets

#cleos get currency balance eosio.token  user1
#cleos get currency balance eosio.token  user2
#