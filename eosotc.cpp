#include "eosotc.hpp"
#include <iostream>
#include <string>
#include <vector>

eosotc::eosotc(account_name self) : contract(self),
                                    m_orders(self, self),
                                    m_markets(self, self)
{
}

void eosotc::hi(account_name user)
{
    print("Hello, ", name{user});
}

void eosotc::clear_db()
{
    for (auto itr = m_orders.begin(); itr != m_orders.end();)
    {
        //prints("id:");
        // printi(itr->id);
        auto temp = itr;
        itr++;
        m_orders.erase(temp);
    }

    for (auto itr = m_markets.begin(); itr != m_markets.end();)
    {
        //  prints("id:");
        //  printi(itr->id);
        auto temp = itr;
        itr++;
        m_markets.erase(temp);
    }
}

void eosotc::create_market(account_name token_contract, const symbol_type &token_symbol)
{
    symbol_type eos{S(4, EOS)};
    eosio_assert(token_symbol != eos, "asset must not be EOS");

    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol.value;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);

    eosio_assert(itr == idx.end(), "token market already created");

    auto pk = m_markets.available_primary_key();
    m_markets.emplace(_self, [&](auto &market) {
        market.id = pk;
        market.token_contract = token_contract;
        market.token_symbol = token_symbol.value;
        market.opened = false;
    });
}

void eosotc::open_market(account_name token_contract, const symbol_type &token_symbol, bool opend)
{
    uint128_t token_id = (uint128_t(token_contract) << 64) | token_symbol.value;
    auto idx = m_markets.template get_index<N(token_id)>();
    auto itr = idx.find(token_id);

    eosio_assert(itr != idx.end(), "token market does not exist");

    m_markets.modify(*itr, 0, [&](auto &m) {
        m.opened = opend;
    });
}

void eosotc::place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, uint64_t token_contract, uint64_t token_symbol)
{
    prints("place_order========");

    // eosio_assert(eos_amount >= 10000, "invalid eos_amount");
    // eosio_assert(token_amount >= 10000, "invalid token_amount");

    auto pk = m_orders.available_primary_key();
    m_orders.emplace(_self, [&](auto &order) {
        order.id = pk;
        order.creator = creator;
        order.type = type;
        order.eos_amount = eos_amount;
        order.token_amount = token_amount;
        order.token_contract = token_contract;
        order.token_symbol = token_symbol;
        order.created_at = current_time(); //
    });

    print(" Database set ");
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

    // transfer must be EOS token from eosio.token
    // eosio_assert(code == N(eosio.token), "transfer not from eosio.token");
    // eosio_assert(t.to == _self, "transfer not made to this contract");
    // symbol_type symbol{S(4, EOS)};
    // eosio_assert(t.quantity.symbol == symbol, "asset must be EOS");
    // eosio_assert(t.quantity.is_valid(), "invalid quantity");

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

    eosio_assert(param.opt > OPT_BEGIN && param.opt < OPT_END, "invalid type");

    if (param.opt == OPT_CREATE_MARKET)
    {
        eosio_assert(t.from == string_to_name(ADMIN), "require_auth admin");
        create_market(code, t.quantity.symbol);
    }
    else if (param.opt == OPT_OPEN_MARKET)
    {
        eosio_assert(t.from == string_to_name(ADMIN), "require_auth admin");
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
            place_order(t.from, BID, t.quantity.amount, param.amount, param.token_contract, param.token_symbol);
        }
        else
        {
            //挂卖单
            place_order(t.from, BID, param.amount, t.quantity.amount, code, t.quantity.symbol);
        }
    }
    else if (param.opt == OPT_TRADE)
    {
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
