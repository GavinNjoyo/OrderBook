#include "OrderBook.hpp"
#include <iostream>
#include <algorithm>
#include <iomanip>

// Order constructor and getters/setters
OrderBook::Order::Order(int id, OrderType type, Side side, int p, int q)
    : OrderId(id), Type(type), side(side), price(p), quantity(q) {}

int OrderBook::Order::getId() const { return OrderId; }
OrderBook::OrderType OrderBook::Order::getType() const { return Type; }
OrderBook::Side OrderBook::Order::getSide() const { return side; }
int OrderBook::Order::getPrice() const { return price; }
int OrderBook::Order::getQuantity() const { return quantity; }
void OrderBook::Order::setPrice(int p) { price = p; }
void OrderBook::Order::setQuantity(int q) { quantity = q; }

// Add order
void OrderBook::addOrder(const Order& order) {
    orders.push_back(order);
    orderMap[order.getId()] = orders.size() - 1;
}

// Cancel order
void OrderBook::cancelOrder(int orderId) {
    auto it = orderMap.find(orderId);
    if (it != orderMap.end()) {
        std::cout << "Cancelled Order ID " << orderId << std::endl;
        orders.erase(orders.begin() + it->second);
        orderMap.erase(it);
        // Update indices in orderMap
        for (size_t i = it->second; i < orders.size(); ++i) {
            orderMap[orders[i].getId()] = i;
        }
    } else {
        std::cout << "Order ID " << orderId << " not found for cancellation." << std::endl;
    }
}

// Match orders
void OrderBook::matchOrder() {
    auto buyOrders = getOrderBySide(Side::Buy);
    auto sellOrders = getOrderBySide(Side::Sell);
    matchOrderLists(buyOrders, sellOrders);
}

// Print one order
void OrderBook::printOrder(const Order& order) {
    std::string typeStr;
    switch (order.getType()) {
        case OrderType::Limit: typeStr = "Limit"; break;
        case OrderType::Market: typeStr = "Market"; break;
        case OrderType::Stop: typeStr = "Stop"; break;
        case OrderType::GoodTillCanceled: typeStr = "GoodTillCanceled"; break;
        case OrderType::FillOrKill: typeStr = "FillOrKill"; break;
        default: typeStr = "Unknown"; break;
    }

    std::string sideStr;
    switch (order.getSide()) {
        case Side::Buy: sideStr = "Buy"; break;
        case Side::Sell: sideStr = "Sell"; break;
        default: sideStr = "Unknown"; break;
    }

    std::cout << "Order ID: " << order.getId()
              << ", Type: " << typeStr
              << ", Side: " << sideStr
              << ", Price: " << order.getPrice()
              << ", Quantity: " << order.getQuantity()
              << std::endl;
}

// Print all orders
void OrderBook::printOrders() {
    if (orders.empty()) {
        std::cout << "Empty Orders" << std::endl;
        return;
    }
    for (const auto& order : orders) {
        printOrder(order);
    }
}

// Price match condition
bool OrderBook::priceMatch(int buyPrice, int sellPrice) {
    return buyPrice >= sellPrice;
}

// Get orders by side (return pointers to original orders for modification)
std::vector<OrderBook::Order*> OrderBook::getOrderBySide(Side side) {
    std::vector<Order*> result;
    for (auto& order : orders) {
        if (order.getSide() == side && order.getQuantity() > 0) {
            result.push_back(&order);
        }
    }
    return result;
}

// Match buy and sell orders
void OrderBook::matchOrderLists(std::vector<Order*>& buys, std::vector<Order*>& sells) {
    for (auto* buy : buys) {
        for (auto* sell : sells) {
            if (priceMatch(buy->getPrice(), sell->getPrice()) && buy->getQuantity() > 0 && sell->getQuantity() > 0) {
                int matchedQuantity = std::min(buy->getQuantity(), sell->getQuantity());
                executeOrder(*buy, *sell, matchedQuantity);
                buy->setQuantity(buy->getQuantity() - matchedQuantity);
                sell->setQuantity(sell->getQuantity() - matchedQuantity);
                if (buy->getQuantity() == 0) break;
            }
        }
    }
}

// Execute matched orders (print trade)
void OrderBook::executeOrder(Order& buy, Order& sell, int quantity) {
    std::cout << "Matched Buy OrderID " << buy.getId()
              << " with Sell OrderID " << sell.getId()
              << " at Quantity " << quantity
              << " at Price " << std::fixed << std::setprecision(2)
              << sell.getPrice() << std::endl;
}
