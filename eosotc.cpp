#include "eosotc.hpp"
#include <iostream>
#include <string>
#include <vector>

eosotc::eosotc(account_name self) : contract(self),
                                    m_orders(self, self)
// thiscontract(self)
{
}

void eosotc::hi(account_name user)
{
    print("Hello, ", name{user});
}

void eosotc::place_order(account_name creator, uint8_t type, uint64_t eos_amount, uint64_t token_amount, std::string token_symbol)
{
    print("place_order");
    eosio_assert(eos_amount >= 10000, "invalid eos_amount");
    eosio_assert(token_amount >= 10000, "invalid token_amount");

    auto pk = m_orders.available_primary_key();
    m_orders.emplace(_self, [&](auto &order) {
        order.id = pk;
        order.creator = creator;
        order.type = type;
        order.eos_amount = eos_amount;
        order.token_amount = token_amount;
        order.token_symbol = token_symbol; // 代币符号
        order.created_at = current_time(); //
    });

    print(" Database set ");
}

int split(vector<string>& ret, const string& str, string sep)
{
    if (str.empty())
    {
        return 0;
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
    return 0;
}

void eosotc::parse_memo_param(string memo, memo_param& param)
{
    prints(string("parse_memo_param:" + memo).c_str());
    vector<string> pairs;
    split(pairs, memo, "&");
    for (int i = 0; i < pairs.size(); i++){
        vector<string> pair;
        split(pair, pairs[i], "=");
        if (pair.size() > 1)
        {
            if (pair[0] == "type")
            {
                param.type = std::stoi(pair[1], nullptr, 0);
            }
            else if (pair[0] == "order_id")
            {
                param.order_id = std::stoull(pair[1], nullptr, 0);
            }
            else if (pair[0] == "amount")
            {
                param.amount = std::stoull(pair[1], nullptr, 0);
            }
            else if (pair[0] == "symbol")
            {
                param.symbol = pair[1];
            }
        }
    }
}

void eosotc::on(const currency::transfer &t, account_name code)
{
    print("eosotc::on");

    // transfer must be EOS token from eosio.token
    eosio_assert(code == N(eosio.token), "transfer not from eosio.token");
    eosio_assert(t.to == _self, "transfer not made to this contract");
    symbol_type symbol{S(4, EOS)};
    eosio_assert(t.quantity.symbol == symbol, "asset must be EOS");
    eosio_assert(t.quantity.is_valid(), "invalid quantity");

    // if transfer amount is 0.0001 EOS then withdraw
    if (t.quantity.amount == 1)
    {
        // withdraw(t.from, t.quantity);
        return;
    }
    memo_param param;
    parse_memo_param(t.memo, param);

    eosio_assert(param.type >= 1 && param.type <= 2, "invalid type");

    if (param.type == PLACE_ORDER)
    {
        place_order(t.from, BID, t.quantity.amount, param.amount, param.symbol);
    }
    else if (param.type == TRADE)
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
