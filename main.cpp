// Low-Latency Order Matching Engine
// Author: Rushikesh Randive
// Date: [Today's Date]

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <chrono>

// Define an Order structure
struct Order {
    int orderId;
    std::string side; // "buy" or "sell"
    double price;
    int quantity;
    std::chrono::steady_clock::time_point timestamp;

    Order(int id, const std::string& s, double p, int q)
        : orderId(id), side(s), price(p), quantity(q), timestamp(std::chrono::steady_clock::now()) {}
};

// Define the OrderBook class
class OrderBook {
private:
    // Price -> Queue of Orders (FIFO per price level)
    std::map<double, std::queue<Order>, std::greater<double>> buyOrders; // High price first
    std::map<double, std::queue<Order>> sellOrders;                      // Low price first

public:
    void addOrder(const Order& order) {
        if (order.side == "buy") {
            buyOrders[order.price].push(order);
        } else {
            sellOrders[order.price].push(order);
        }
        std::cout << "Added order ID " << order.orderId << " (" << order.side << ") at price " << order.price << "\n";
    }

    void matchOrders() {
        while (!buyOrders.empty() && !sellOrders.empty()) {
            auto highestBuy = buyOrders.begin();
            auto lowestSell = sellOrders.begin();

            if (highestBuy->first >= lowestSell->first) {
                Order& buyOrder = highestBuy->second.front();
                Order& sellOrder = lowestSell->second.front();

                int tradeQty = std::min(buyOrder.quantity, sellOrder.quantity);
                std::cout << "Matched Buy ID " << buyOrder.orderId
                          << " with Sell ID " << sellOrder.orderId
                          << " for quantity " << tradeQty
                          << " at price " << sellOrder.price << "\n";

                buyOrder.quantity -= tradeQty;
                sellOrder.quantity -= tradeQty;

                if (buyOrder.quantity == 0)
                    highestBuy->second.pop();
                if (sellOrder.quantity == 0)
                    lowestSell->second.pop();

                if (highestBuy->second.empty())
                    buyOrders.erase(highestBuy);
                if (lowestSell->second.empty())
                    sellOrders.erase(lowestSell);
            } else {
                break;
            }
        }
    }

    // TODO: Implement cancelOrder(orderId)
};

int main() {
    OrderBook ob;

    // Example orders
    ob.addOrder(Order(1, "buy", 100.5, 10));
    ob.addOrder(Order(2, "sell", 99.5, 5));
    ob.addOrder(Order(3, "sell", 100.0, 7));

    ob.matchOrders();

    return 0;
}
