
创建市场

 ```
cleos push action dddddddadmin transfer '["dddddddadmin", "ddddcontract", "0.0001 DDD", "opt=create_market" ]' -p dddddddadmin

```

打开市场
```
cleos push action dddddddadmin transfer '["dddddddadmin", "ddddcontract", "0.0001 DDD", "opt=open_market" ]' -p dddddddadmin
```

查看市场
```
cleos get table ddddcontract ddddcontract markets
```

挂买单：用2个EOS买1个DDD 
```
cleos  transfer ddddddduser1  ddddcontract  '2.0000 EOS' 'opt=place_bid_order&amount=10000&token_contract=5355506343930010928&token_symbol=1145324548'
```

查看订单
```
cleos get table ddddcontract ddddcontract bidorders
```

吃单
```
cleos push action dddddddadmin transfer '["ddddddduser2", "ddddcontract", "1.0000 DDD", "opt=sell_token&order_id=1536753786500000" ]' -p ddddddduser2

```
查看手续费
```
cleos get table ddddcontract ddddcontract fees
```

挂单：卖5个DDD，价格是1个EOS
```
cleos push action dddddddadmin transfer '["ddddddduser1", "ddddcontract", "5.0000 DDD", "opt=place_ask_order&amount=10000" ]' -p ddddddduser1
```

查看订单
```
cleos get table ddddcontract ddddcontract askorders
```

吃单
```
cleos  transfer ddddddduser2  ddddcontract  '1.0000 EOS' 'opt=buy_token&order_id=1536753828500000'
```

提取手续费
 ```
cleos  transfer dddddddadmin  ddddcontract  '0.0001 EOS' 'opt=take_fee&limit=1'
```
