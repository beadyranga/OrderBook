#include "Order.h"
#include "Parser.h"
#include "OrderBook.h"
#include "MarketDataProvider.h"

#ifndef MAIN_C
#define MAIN_C

using namespace std;

int main(int argc, char* argv[]) {
//    string cmdLine;
//    while (getline(cin, cmdLine)) {
//        // Getting the Order Object
//        Order* newOrder;
//        unsigned long int prevTimestamp = 0;
//        try {
//            newOrder = trading::Parser::parse(cmdLine);
//            std::cout << "Order Submitted "<< newOrder->getSymbol() << std::endl;
//            if (newOrder->getTimestamp() < prevTimestamp) { // out of order messages
//                throw trading::OutOfOrder(); // technically, this throw should be elsewhere in a real system
//            }
//
//        } catch (const trading::ParseException &) {
//            continue;
//        }
//
//        try {
//            trading::OrderBook::getInstance().processOrder(*newOrder);
//        } catch (const trading::OrderBookException&) {
//            continue;
//        }
//    }
//
//    return 0;

    try {

        unsigned long int targetSize;
        bool useFileForMarketFeed;


        switch (argc) {
            case 2:
                targetSize = std::atol(argv[1]);
                if (targetSize < 1) {
                    abort();
                }
                useFileForMarketFeed = false;
                break;
            case 3:
                targetSize = std::atol(argv[1]);
                if (targetSize < 1) {
                    abort();
                }
                useFileForMarketFeed = true;
                try {
                    trading::MarketDataProvider::getInstance().readMarketDataFile(argv[2]);
                } catch (const trading::BadMarketDataFile& e) {

                    abort();
                }
                break;
            default:
//                FILE_LOG(logERROR) << "Error with program arguments. There are two ways to start this program:";
//                FILE_LOG(logERROR) << "./Pricer 200             // 200 is the target size of market order";
//                FILE_LOG(logERROR) << "./Pricer 200 feed.txt    // use feed.txt instead of standard input";
                abort();
        }

        assert(targetSize >= 1);

        // For the the market order
        double newAmount = 0;
        double cachedBuyAmount = 0;
        double cachedSellAmount = 0;

        // Main loop
        while (true) {

            // Infinite loop if using standard input; break loop on EOF if using a market data file
            if (useFileForMarketFeed && !trading::MarketDataProvider::getInstance().hasNextMessage()) {
                break;
            }

            // "Receive" new message
            std::string msg;
            if (useFileForMarketFeed) {
                msg = trading::MarketDataProvider::getInstance().nextMessage();
                std::cout << msg << std::endl;
            } else {
                std::getline(std::cin, msg);
                // std::cout << msg << std::endl;
            }


            Order* newOrder;
            unsigned long int prevTimestamp = 0;
            try {
                newOrder = trading::Parser::parse(msg);
                newOrder->setExecutedPrice(0);
                newOrder->setRemainingQuantity(newOrder->getOrderQuantity());
                newOrder->setExecType('0');
                newOrder->setTotalExecutedQuantity(0);
                newOrder->setExecutedSize(0);

                std::cout << "Order Submitted " << newOrder->getSymbol() << std::endl;
                if (newOrder->getTimestamp() < prevTimestamp) { // out of order messages
                    throw trading::OutOfOrder(); // technically, this throw should be elsewhere in a real system
                }

            } catch (const trading::ParseException &) {
                continue;
            }

            try {
                trading::OrderBook::getInstance().processOrder(*newOrder);
            } catch (const trading::OrderBookException &) {
                continue;
            }
        }

    } catch (const trading::Exception& e) { // exception catch-all
    }

}

#endif
//
//Created by wijekoonw on 4/17/2019.
//
