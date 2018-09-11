#!/bin/bash

cleos wallet unlock --password PW5K9UYDNxyBrSHyruWdLywwRghfMmd1u9CUaayqYSjBZL7qLiwBy

#清空数据
cleos  transfer user1  eosotc  '1 DDD' 'opt=clear_db'

cleos get table eosotc eosotc askorders
cleos get table eosotc eosotc bidorders
cleos get table eosotc eosotc markets
cleos get table eosotc eosotc fees

#创建市场
cleos  transfer user1  eosotc  '1 DDD' 'opt=create_market'

#打开市场
cleos  transfer user1  eosotc  '1 DDD' 'opt=open_market'

#leos get table eosotc eosotc markets

#挂买单：用2个EOS买1个DDD 
cleos  transfer user1  eosotc  '2 EOS' 'opt=place_bid_order&amount=10000&token_contract=6138663591592764928&token_symbol=1145324548'
cleos get table eosotc eosotc bidorders

#吃单
#cleos  transfer user2  eosotc  '1 DDD' 'opt=sell_token&order_id=1536634505000000'
#cleos get table eosotc eosotc bidorders
#cleos get table eosotc eosotc fees

#挂单：卖5个DDD，价格是1个EOS
cleos  transfer user1  eosotc  '5 DDD' 'opt=place_ask_order&amount=10000'
cleos get table eosotc eosotc askorders

#吃单
#cleos  transfer user2  eosotc  '1 EOS' 'opt=buy_token&order_id=1536630964500000'
#cleos get table eosotc eosotc bidorders
#cleos get table eosotc eosotc fees

# 提取手续费
#cleos  transfer user1  eosotc  '1 DDD' 'opt=take_fee&limit=1'


#cleos get currency balance eosio.token  user1
#cleos get currency balance eosio.token  user2
#