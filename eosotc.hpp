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
  uint8_t type;
  uint64_t order_id;
  uint64_t amount;
  std::string symbol;
};

class eosotc : public eosio::contract
{
public:
  eosotc(account_name self);
  /// @abi action
  void hi(account_name user);

  void apply(account_name contract, account_name action);

private:
  //using contract::contract;

  // @abi table
  struct order
  {
    uint64_t id;
    account_name creator;
    uint8_t type;             // 1:ask 2:bid
    uint64_t eos_amount;      // eos数量
    uint64_t token_amount;    // 代币数量
    std::string token_symbol; // 代币符号
    uint64_t created_at;

    auto primary_key() const { return id; }
    EOSLIB_SERIALIZE(order, (id)(creator)(type)(eos_amount)(token_amount)(token_symbol)(created_at))
  };
  typedef eosio::multi_index<N(order), order> orders;

  void on(const currency::transfer &t, account_name code);
  void place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, std::string token_symbol);
  void parse_memo_param(string memo, memo_param& param);

private:
  // contract name
  // account_name _thiscontract;
  orders m_orders;
};
