#include "eosotc.hpp"
#include <iostream>
#include <string>
#include <vector>

#define DEBUG

#ifdef DEBUG
#define ASSERT(test, msg) \
    if ((test) <= 0)      \
    {                     \
        prints(msg);      \
        return;           \
    }
#else
#define ASSERT(test, msg) eosio_assert(test, msg);
#endif

eosotc::eosotc(account_name self) : contract(self),
                                    m_ask_orders(self, self),
                                    m_bid_orders(self, self),
                                    m_markets(self, self)
{
}

void eosotc::hi(account_name user)
{
    print("Hello, ", name{user});
}

void eosotc::clear_db()
{
    for (auto itr = m_ask_orders.begin(); itr != m_ask_orders.end();)
    {
        auto temp = itr;
        itr++;
        m_ask_orders.erase(temp);
    }

    for (auto itr = m_bid_orders.begin(); itr != m_bid_orders.end();)
    {
        auto temp = itr;
        itr++;
        m_bid_orders.erase(temp);
    }

    for (auto itr = m_markets.begin(); itr != m_markets.end();)
    {
        auto temp = itr;
        itr++;
        m_markets.erase(temp);
    }
}

void eosotc::create_market(account_name token_contract, const symbol_type &token_symbol)
{
    symbol_type eos{S(4, EOS)};
    ASSERT(token_symbol != eos, "asset must not be EOS");

    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol.value;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);

    ASSERT(itr == idx.end(), "market already created");

    auto pk = m_markets.available_primary_key();
    m_markets.emplace(_self, [&](auto &market) {
        market.id = pk;
        market.token_contract = token_contract;
        market.token_symbol = token_symbol.value;
        market.opened = false;
    });
}

void eosotc::open_market(account_name token_contract, const symbol_type &token_symbol, bool open)
{
    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol.value;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);

    ASSERT(itr != idx.end(), "market does not exist");

    m_markets.modify(*itr, 0, [&](auto &m) {
        m.opened = open;
    });
}

void eosotc::place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, uint64_t token_contract, uint64_t token_symbol)
{
    prints(" place_order========");

    //至少一个eos
    ASSERT(eos_amount >= 10000, "invalid eos_amount");
    ASSERT(token_amount >= 10000, "invalid token_amount");

    //market存在，且打开状态
    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);
    ASSERT(itr != idx.end(), " market does not exist");
    ASSERT(itr->opened, "market does not opened");

    auto pk = type == ASK ? m_ask_orders.available_primary_key() : m_bid_orders.available_primary_key();
    auto insert = [&](auto &order) {
        order.id = pk;
        order.creator = creator;
        order.eos_amount = eos_amount;
        order.token_amount = token_amount;
        order.token_contract = token_contract;
        order.token_symbol = token_symbol;
        order.created_at = current_time();
    };
    if (type == ASK)
    {
        m_ask_orders.emplace(_self, insert);
    }
    else
    {
        m_bid_orders.emplace(_self, insert);
    }
    print(" Database set ");
}

void eosotc::buy_token(account_name buyer, uint64_t order_id, uint64_t eos_amount, uint64_t token_contract, uint64_t token_symbol)
{
    prints(" buy_token========");

    //market存在，且打开状态
    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);
    ASSERT(itr != idx.end(), "market does not exist");
    ASSERT(itr->opened, "market does not opened");

    auto order_itr = m_ask_orders.find(order_id);
    ASSERT(order_itr != m_ask_orders.end(), "token does not exist");
    const auto &order = *order_itr;
    ASSERT(order.token_contract == token_contract, "token contract not match");
    ASSERT(order.token_symbol == token_symbol, "token symbol not match");
    ASSERT(order.eos_amount == eos_amount, "eos_amount not match");

    account_name exchanger = _self;
    account_name seller = order.creator;
    symbol_type eos{S(4, EOS)};

    int64_t eos_fee = FEE_RATE * eos_amount / 100;
    int64_t token_fee = FEE_RATE * order.token_amount / 100;
    int64_t real_eos_amount = eos_amount - token_fee;
    int64_t real_token_amount = order.token_amount - token_fee;

    //发送token给买方
    action(
        permission_level{exchanger, N(active)},
        order.token_contract, N(transfer),
        std::make_tuple(exchanger, buyer, asset{real_token_amount, token_symbol}, std::string("receive token from EOSOTC")))
        .send();

    //发送EOS给卖方
    action(
        permission_level{exchanger, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(exchanger, seller, asset{real_eos_amount, eos.value}, std::string("receive EOS from EOSOTC")))
        .send();

    m_ask_orders.erase(order_itr);
}

void eosotc::sell_token(account_name seller, uint64_t order_id, uint64_t token_amount, uint64_t token_contract, uint64_t token_symbol)
{
    prints(" sell_token========");

    //market存在，且打开状态
    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);
    ASSERT(itr != idx.end(), "market does not exist");
    ASSERT(itr->opened, "market does not opened");

    //卖token
    auto order_itr = m_bid_orders.find(order_id);
    ASSERT(order_itr != m_bid_orders.end(), "token does not exist");
    const auto &order = *order_itr;
    ASSERT(order.token_contract == token_contract, "token contract not match");
    ASSERT(order.token_symbol == token_symbol, "token symbol not match");
    ASSERT(order.token_amount == token_amount, "token_amount not match");

    account_name exchanger = _self;
    account_name buyer = order.creator;
    symbol_type eos{S(4, EOS)};

    int64_t eos_fee = FEE_RATE * order.eos_amount / 100;
    int64_t token_fee = FEE_RATE * token_amount / 100;
    int64_t real_eos_amount = order.eos_amount - token_fee;
    int64_t real_token_amount = token_amount - token_fee;

    //发送token给买方
    action(
        permission_level{exchanger, N(active)},
        order.token_contract, N(transfer),
        std::make_tuple(exchanger, buyer, asset{real_token_amount, token_symbol}, std::string("receive token from EOSOTC")))
        .send();

    //发送EOS给卖方
    action(
        permission_level{exchanger, N(active)},
        N(eosio.token), N(transfer),
        std::make_tuple(exchanger, seller, asset{real_eos_amount, eos.value}, std::string("receive EOS from EOSOTC")))
        .send();

    m_bid_orders.erase(order_itr);
}

void split(vector<string> &ret, const string &str, string sep)
{
    if (str.empty())
    {
        return;
    }
    string tmp;
    string::size_type pos_begin = str.find_first_not_of(sep);
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos)
    {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos)
        {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        }
        else
        {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty())
        {
            ret.push_back(tmp);
            tmp.clear();
        }
    }
}

void eosotc::parse_memo_param(string memo, memo_param &param)
{
    prints(string("[eosotc::parse_memo_param] " + memo).c_str());
    prints("=============================================");

    //memo_param param;
    vector<string> pairs;
    split(pairs, memo, "&");
    for (int i = 0; i < pairs.size(); i++)
    {
        vector<string> pair;
        split(pair, pairs[i], "=");
        if (pair.size() > 1)
        {
            if (pair[0] == "opt")
            {
                param.opt = std::stoi(pair[1], nullptr, 0);
            }
            else if (pair[0] == "order_id")
            {
                param.order_id = std::stoull(pair[1], nullptr, 0);
            }
            else if (pair[0] == "amount")
            {
                param.amount = std::stoull(pair[1], nullptr, 0);
            }
            else if (pair[0] == "token_contract")
            {
                param.token_contract = std::stoull(pair[1], nullptr, 0);
            }
            else if (pair[0] == "token_symbol")
            {
                param.token_symbol = std::stoull(pair[1], nullptr, 0);
            }
        }
    }
}

void eosotc::on(const currency::transfer &t, account_name code)
{
    prints(string("[eosotc::on] " + t.memo).c_str());
    prints("=============================================");
    ASSERT(t.quantity.is_valid(), "invalid quantity");

    memo_param param;
    parse_memo_param(t.memo, param);

    prints(string("[eosotc::on]").c_str());

    prints(" t.from:");
    printui(t.from);

    prints(" t.to:");
    printui(t.to);
    prints(" t.quantity.amount:");
    printui(t.quantity.amount);

    prints(" param.opt:");
    printui(param.opt);
    prints(" param.order_id:");
    printui(param.order_id);
    prints(" param.amount:");
    printui(param.amount);
    prints(" param.token_contract:");
    printui(param.token_contract);
    prints(" param.token_symbol:");
    printui(param.token_symbol);
    prints("=============================================");

    if (param.opt == uint8_t(99))
    {
        clear_db();
        return;
    }

    ASSERT(param.opt > OPT_BEGIN && param.opt < OPT_END, "invalid type");

    if (param.opt == OPT_CREATE_MARKET)
    {
        ASSERT(t.from == string_to_name(ADMIN), "require_auth admin");
        create_market(code, t.quantity.symbol);
    }
    else if (param.opt == OPT_OPEN_MARKET)
    {
        ASSERT(t.from == string_to_name(ADMIN), "require_auth admin");
        open_market(code, t.quantity.symbol, true);
    }
    else if (param.opt == OPT_CLOSE_MARKET)
    {
        eosio_assert(t.from == string_to_name(ADMIN), "require_auth admin");
        open_market(code, t.quantity.symbol, false);
    }
    else if (param.opt == OPT_PLACE_ORDER)
    {
        symbol_type eos{S(4, EOS)};
        if (code == N(eosio.token) && eos == t.quantity.symbol)
        {
            //挂买单
            ASSERT(param.amount >= 10000, "invalid amount");
            ASSERT(param.token_contract > 0, "invalid token_contract");
            ASSERT(param.token_symbol > 0, "invalid token_symbol");

            place_order(t.from, BID, t.quantity.amount, param.amount, param.token_contract, param.token_symbol);
        }
        else
        {
            //挂卖单
            ASSERT(param.amount >= 10000, "invalid amount");
            place_order(t.from, ASK, param.amount, t.quantity.amount, code, t.quantity.symbol.value);
        }
    }
    else if (param.opt == OPT_TRADE)
    {
        //吃单
        symbol_type eos{S(4, EOS)};
        if (code == N(eosio.token) && eos == t.quantity.symbol)
        {
            buy_token(t.from, param.order_id, t.quantity.amount, param.token_contract, param.token_symbol);
        }
        else
        {
            sell_token(t.from, param.order_id, t.quantity.amount, param.token_contract, param.token_symbol);
        }
    }
}

void eosotc::apply(account_name code, account_name action)
{
    if (action == N(transfer))
    {
        on(unpack_action_data<currency::transfer>(), code);
        return;
    }
    if (code != _self)
        return;

    auto &thiscontract = *this;
    switch (action)
    {
        EOSIO_API(eosotc, (hi)(on));
    };
}

extern "C"
{
    [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        eosotc otc(receiver);
        otc.apply(code, action);
        eosio_exit(0);
    }
}
