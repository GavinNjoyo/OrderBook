#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    std::string input;
    std::cout << "Enter order as: <OrderId> <OrderType> <Side> <Price> <Quantity>" << std::endl;
    std::cout << "Example: 1 Limit Buy 100 10" << std::endl;
    std::cout << "Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "exit") break;

        int n = sendto(sockfd, input.c_str(), input.size(), 0,
                       (const struct sockaddr *) &servaddr, sizeof(servaddr));
        if (n < 0) {
            perror("sendto failed");
        }
    }

    close(sockfd);
    return 0;
}
