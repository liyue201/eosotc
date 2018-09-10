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

const static char *ADMIN = "user1";

enum opt
{
  OPT_BEGIN = 0,
  OPT_CREATE_MARKET,
  OPT_OPEN_MARKET,
  OPT_CLOSE_MARKET,
  OPT_PLACE_ORDER,
  OPT_TRADE,
  OPT_END
};

const static uint8_t ASK = 1;
const static uint8_t BID = 2;
const static uint64_t FEE_RATE = 1;

struct memo_param
{
  uint8_t opt = 0;
  uint64_t order_id = 0;
  uint64_t amount = 0;
  uint64_t token_contract = 0;
  uint64_t token_symbol = 0;
};

class eosotc : public eosio::contract
{
public:
  eosotc(account_name self);
  /// @abi action
  void hi(account_name user);

  void apply(account_name contract, account_name action);

public:
  // @abi table markets
  struct market
  {
    uint64_t id = 0;
    uint64_t token_contract = 0;
    uint64_t token_symbol = 0;
    bool opened = false;

    auto primary_key() const { return id; }
    uint128_t by_token_id() const { return (uint128_t(token_contract) << 64) | token_symbol; }
    EOSLIB_SERIALIZE(market, (id)(token_contract)(token_symbol)(opened))
  };
  typedef eosio::multi_index<N(markets), market, indexed_by<N(token_id), const_mem_fun<market, uint128_t, &market::by_token_id>>> markets;

  // @abi table askorders
  // @abi table bidorders
  struct order
  {
    uint64_t id = 0;
    account_name creator = 0;
    uint64_t eos_amount = 0;
    uint64_t token_amount = 0;
    uint64_t token_contract = 0;
    uint64_t token_symbol = 0;
    uint64_t created_at = 0;
    auto primary_key() const { return id; }
    EOSLIB_SERIALIZE(order, (id)(creator)(eos_amount)(token_amount)(token_contract)(token_symbol)(created_at))
  };
  typedef eosio::multi_index<N(askorders), order> ask_orders;
  typedef eosio::multi_index<N(bidorders), order> bid_orders;

  void clear_db();

  void parse_memo_param(string memo, memo_param &param);

  void on(const currency::transfer &t, account_name code);

  void create_market(account_name token_contract, const symbol_type &token_symbol);

  void open_market(account_name token_contract, const symbol_type &token_symbol, bool open);

  void place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, uint64_t token_contract, uint64_t token_symbol);

  void buy_token(uint64_t order_id, account_name buyer, uint64_t eos_amount, uint64_t token_contract, uint64_t token_symbol);

  void sell_token(uint64_t order_id, account_name seller, uint64_t token_amount, uint64_t token_contract, uint64_t token_symbol);

private:
  ask_orders m_ask_orders;
  bid_orders m_bid_orders;
  markets m_markets;
};
