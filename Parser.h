//
//#ifndef PARSER_H_
//#define PARSER_H_
#pragma once
#include <sstream>
#include <iostream>
#include <vector>

#include "Order.h"
#include "Exceptions.h"
#include "Utils.h"

namespace trading {

    class Parser {
    public:
        // Basic Order Parser
        static Order* parse(const std::string &msg);
    };

} // end of namespace


// Definitions of inline functions

inline Order* trading::Parser::parse(const std::string &msg) {
    // Get fields
    std::vector<std::string> fields = tokenize(msg, ' ');
//    std::cout << "\n Parse : Inside parse method "<< ' ';
    // Basic check
//    if (fields.size() != 4) {
//        if (fields.size() != 10) {
//            throw BadParse();
//        }
//    }

//    for (std::vector<std::string>::const_iterator i = fields.begin(); i != fields.end(); ++i)
//        std::cout << "+++++"<<*i << ' ';


    Order* order = new Order();

    // Parse order type
    std::string orderType = fields.at(1);
    if (orderType.compare("A") == 0) {
        // Add Order    234343434 A IBM BUY LIMIT DAY 200 10.5 TR1 Order1 (A can be M: Modify C: Cancel )
        //					0	  1	  2  3	  4	   5   6	7    8    9

        // Add Timestamp
        order->setTimestamp(std::atol(fields.at(0).c_str()));
        // Add Order Msg Type (ADD : 1, AMEND : 2, CANCEL :3)
        order->setMsgType(1);
        // Add Symbol : IBM
        order->setSymbol(fields.at(2).c_str());
        //ADD Order Side (SELL : 2 OR BUY :1)
        if (fields.at(3) =="BUY") {
//            std::cout << "\n Parse : Buy"<< fields.at(3) << ' ';
            order->setSide(1);
        } else {
//            std::cout << "\n Parse : Sell"<< fields.at(3)  <<' ';
            order->setSide(2);
        }
        //Add Order Type (LIMIT OR MARKET);
        if (fields.at(4)=="MARKET") {
//            std::cout << "\n Parse : Market"<< ' ';
            order->setOrderType(2);
        } else {
            //LIMIT buy default
//            std::cout << "\n Parse : Limit"<< ' ';
            order->setOrderType(1);
        }

        //Add Order Tif (DAY OR IOC);
        if (fields.at(5) == "IOC") {
//            std::cout << "\n Parse : IOC"<< ' ';
            order->setTif(2);
        } else {
            // Day by default
//            std::cout << "\n Parse : DAY"<< ' ';
            order->setTif(1);
        }

        //Add Quantity
//        std::cout << "\n Parse : Quantity : "<< std::stoi(fields.at(6)) << ' ';
        order->setOrderQuantity(std::stoi(fields.at(6)));

        //Add Order Price
        double price = std::stod(fields.at(7));
//        std::cout << "\n Parse : Price : "<< price<< ' ';
        order->setPrice(price);

        //Add User Id
        order->setUserID(fields.at(8));

        //Add Client Order ID
        order->setClientOrderID(fields.at(9));

    } else if (orderType.compare("M") == 0) {
        // 234343434 M IBM 30 10.5 TR1 Order2 Order1
        // Add Timestamp
        order->setTimestamp(std::atol(fields.at(0).c_str()));
        // Add Order Msg Type (ADD(A) : 1, AMEND(M) : 2, CANCEL(C) :3)
        order->setMsgType(2);

        //set symbol for modify
        order->setSymbol(fields.at(2).c_str());

        //Add Quantity
        order->setOrderQuantity(std::stoi(fields.at(3)));

        //Add Order Price
        double price = std::stod(fields.at(4));
        order->setPrice(price);

        //Add User Id
        order->setUserID(fields.at(5));
        //Add Client Order ID
        order->setClientOrderID(fields.at(6));

        order->setOriginalClientOrderID(fields.at(7));

    } else if (orderType.compare("C") == 0) {
        // 234343434 C IBM TR1 Order2 Order1

        std::cout << "\n Parse : Inside Cancel Order "<< ' ';

        // Add Timestamp
        order->setTimestamp(std::atol(fields.at(0).c_str()));
        // Add Order Msg Type (ADD(A) : 1, AMEND(M) : 2, CANCEL(C) :3)
        order->setMsgType(3);

        //set symbol for modify
        order->setSymbol(fields.at(2).c_str());

        //Add User Id
        order->setUserID(fields.at(3).c_str());
        //Add Client Order ID
//        std::cout << "\n Parse : Client Order Id : "<< fields.at(4)<< ' ';
        order->setClientOrderID(fields.at(4).c_str());

//        std::cout << "\n Parse : Original Client Order Id : "<< fields.at(5)<< ' ';
        order->setOriginalClientOrderID(fields.at(5).c_str());
    } else { // bad parse
        std::cout << "===========BAD PARSE==========="<< order->getPrice() << std::endl;
//        throw BadParse();
    }

    return order;
}
//
//#endif /* PARSER_H_ */
