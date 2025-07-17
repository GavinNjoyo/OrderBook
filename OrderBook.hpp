#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include <unordered_map>
#include <cstddef>    // for size_t

class OrderBook {
public:
    enum class OrderType { Market, Limit, Stop, GoodTillCanceled, FillOrKill };
    enum class Side { Buy, Sell };

    struct Order {
        Order(int id, OrderType type, Side side, int price, int quantity);

        int getId() const;
        OrderType getType() const;
        Side getSide() const;
        int getPrice() const;
        int getQuantity() const;

        void setPrice(int);
        void setQuantity(int);

    private:
        int OrderId;
        OrderType Type;
        Side side;
        int price;
        int quantity;
    };

    void addOrder(const Order& order);
    void cancelOrder(int orderId);
    void matchOrder();
    void printOrders();

private:
    std::vector<Order> orders;
    std::unordered_map<int, size_t> orderMap; // map orderId to index in vector

    void printOrder(const Order& order);
    bool priceMatch(int buyPrice, int sellPrice);
    std::vector<Order*> getOrderBySide(Side side);
    void matchOrderLists(std::vector<Order*>& buys, std::vector<Order*>& sells);
    void executeOrder(Order& buy, Order& sell, int quantity);
};

#endif // ORDERBOOK_HPP
