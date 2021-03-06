#pragma once

#include "utils/Common.h"
using namespace common;

class Parser
{
public:
    enum class Action : char
    {
        ADD = 'A',
        CANCEL = 'X',
        MODIFY = 'M',
        TRADE = 'T',
    };
    
    enum class Side : char
    {
        BUY = 'B',
        SELL = 'S',
    };
    
public:
    Parser() = default;
    ~Parser() = default;
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    
    bool parse(const char* str, size_t len, Errors& errors, const int verbose = 0);
    
    auto getAction() { return action_; }
    auto getOrderId() { return orderId_; }
    auto getSide() { return side_; }
    auto getPrice() { return price_; }
    auto getQty() { return qty_; }
    
private:
    char action_ = 0;
    OrderId orderId_ = 0;
    char side_ = 0;
    Price price_ = 0.0;
    Quantity qty_ = 0;
};

