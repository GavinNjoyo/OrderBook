#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstring>   // For memset
#include <unistd.h>  // For close()
#include <arpa/inet.h>  // For socket functions
#include "OrderBook.hpp"

#define PORT 8080
#define BUFFER_SIZE 1024

// Convert string to lowercase
std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

// Convert string to OrderType (case-insensitive)
OrderBook::OrderType strToOrderType(const std::string& s) {
    std::string type = toLower(s);
    if (type == "market") return OrderBook::OrderType::Market;
    if (type == "limit") return OrderBook::OrderType::Limit;
    if (type == "stop") return OrderBook::OrderType::Stop;
    if (type == "goodtillcanceled") return OrderBook::OrderType::GoodTillCanceled;
    if (type == "fillorkill") return OrderBook::OrderType::FillOrKill;
    throw std::runtime_error("Unknown order type");
}

// Convert string to Side (case-insensitive)
OrderBook::Side strToSide(const std::string& s) {
    std::string side = toLower(s);
    if (side == "buy") return OrderBook::Side::Buy;
    if (side == "sell") return OrderBook::Side::Sell;
    throw std::runtime_error("Unknown side");
}

// Parse an order from input string
bool parseOrder(const std::string& input, OrderBook::Order& order) {
    int id, price, quantity;
    std::string typeStr, sideStr;
    std::istringstream iss(input);
    if (!(iss >> id >> typeStr >> sideStr >> price >> quantity))
        return false;
    try {
        order = OrderBook::Order(id, strToOrderType(typeStr), strToSide(sideStr), price, quantity);
    } catch (...) {
        return false;
    }
    return true;
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    OrderBook ob;

    std::cout << "UDP Server running on port " << PORT << std::endl;

    while (true) {
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE - 1,
                         0, (struct sockaddr *) &cliaddr, &len);
        if (n < 0) {
            perror("recvfrom error");
            continue;
        }
        buffer[n] = '\0';
        std::string msg(buffer);
        std::cout << "Received: " << msg << std::endl;

        OrderBook::Order order(0, OrderBook::OrderType::Limit, OrderBook::Side::Buy, 0, 0);
        if (!parseOrder(msg, order)) {
            std::cerr << "Invalid order format: " << msg << std::endl;
            continue;
        }

        ob.addOrder(order);
        ob.matchOrder();
        ob.printOrders();
        std::cout << "-------------------------" << std::endl;
    }

    close(sockfd);
    return 0;
}
