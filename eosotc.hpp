/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>
#include <string>

using namespace eosio;
using std::string;
using std::vector;

const static uint8_t PLACE_ORDER = 1;
const static uint8_t TRADE = 2;

const static uint8_t ASK = 1;
const static uint8_t BID = 2;

struct memo_param
{
  uint8_t opt = 0;
  uint64_t order_id = 0;
  uint64_t amount = 0;
  uint128_t token_id = 0;
};

class eosotc : public eosio::contract
{
public:
  eosotc(account_name self);
  /// @abi action
  void hi(account_name user);

  void apply(account_name contract, account_name action);

public:
  //using contract::contract;

  // @abi table
  struct market
  {
    uint64_t id = 0;
    uint128_t token_id;
    account_name contract = 0;
    string symbol;
    bool enabled = true;

    auto primary_key() const { return id; }
    uint128_t by_token_id() const { return token_id; }
    EOSLIB_SERIALIZE(market, (id)(token_id)(contract)(symbol)(enabled))
  };
  typedef eosio::multi_index<N(markets), market, indexed_by<N(token_id), const_mem_fun<market, uint128_t, &market::by_token_id>>> markets;

  // @abi table
  struct order
  {
    uint64_t id;
    account_name creator;
    uint8_t type;          // 1:ask 2:bid
    uint64_t eos_amount;   // eos数量
    uint64_t token_amount; // 代币数量
    uint128_t token_id;
    uint64_t created_at;
    auto primary_key() const { return id; }
    EOSLIB_SERIALIZE(order, (id)(creator)(type)(eos_amount)(token_amount)(token_id)(created_at))
  };
  typedef eosio::multi_index<N(orders), order> orders;

  void on(const currency::transfer &t, account_name code);
  void place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, uint128_t token_id);
  void parse_memo_param(string memo, memo_param &param);

private:
  orders m_orders;
  markets m_markets;
};
