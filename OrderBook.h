//
// Created by wijekoonw on 4/11/2019.
//

//#ifndef RANGANEW_ORDERBOOK_H
//#define RANGANEW_ORDERBOOK_H

#pragma once

#include <map>
#include <list>
#include <unordered_map>
#include <string>
#include <sstream>
#include <functional>
#include <limits>
#include <algorithm>
#include <cassert>

#include "Exceptions.h"
#include "Order.h"
#include "EnumData.h"


namespace trading {

/**
 * Order Book (for one Instrument) as  Singleton
 */
    class OrderBook {
    public:
        // Singleton
        static OrderBook &getInstance();

        // Print order book
        static std::string printBook();

        // Process a new market/limit order
        static void processOrder(Order &order);

        static void SendExecutionReport(Order &order, string execMsg, char exeType, double totalExcQuantity,
                                        double remainingQuantity, double execPrice, double execSize, string reason);

    private:
        // Iterators
        typedef std::multimap<double, Order>::iterator MapIter;
        typedef std::multimap<double, Order>::reverse_iterator MapReverseIter;
        typedef std::tr1::unordered_map<string, MapIter>::iterator HashMapIter;

        // Multimaps: price -> order
        typedef std::multimap<double, Order, std::greater<double> > BidsMap;
        typedef std::multimap<double, Order, std::less<double> > AsksMap;

        typedef std::tr1::unordered_map<string, MapIter> BidsHash;
        typedef std::tr1::unordered_map<string, MapIter> AsksHash;

        static BidsMap buySideMap; // only declare (define in .cpp)
        static AsksMap sellSideMap;

        static BidsHash buySideHashMap; // only declare (define in .cpp)
        static AsksHash sellSideHashMap;

        void matchOrdersWithBuySide(Order &order);

        void matchOrdersWithSellSide(Order &order);

        OrderBook() {};

        OrderBook(OrderBook const &);      // Don't Implement
        void operator=(OrderBook const &); // Don't implement

    };

}


// Definitions of inline functions

inline trading::OrderBook &trading::OrderBook::getInstance() {
    static OrderBook _instance; // Guaranteed to be destroyed. Instantiated on first use.
    return _instance;
}

inline void trading::OrderBook::processOrder(Order &order) {

    // Pair for the tree
    std::pair<double, Order> priceOrderPair = std::pair<double, Order>(order.getPrice(), order);
    // Map Iterator
    MapIter mapIter; // for the return value when inserting into multimap
    // Pair for the hashmap
    std::pair<string, MapIter> idIterPair;
    //hash map iterator
    HashMapIter hashMapIter;

    switch (order.getMsgType()) {
        case EnumData::NEW_ORDER: // new order
            switch (order.getSide()) {
                case EnumData::Buy:
                    if (!buySideHashMap.empty() &&
                        ((hashMapIter = buySideHashMap.find(order.getClientOrderID())) != buySideHashMap.end())) {
                        //Error you cannot add duplicate client order ID's
                        string errMsg = "Rejected :: You cannot Add Duplicate Client Order ID's for the Order";
                        trading::OrderBook::SendExecutionReport(order, errMsg, '8', order.getTotalExecutedQuantity(),
                                                                order.getRemainingQuantity(), order.getExecutedPrice(),
                                                                order.getExecutedSize(),
                                                                "Duplicate Client ID's in the Order");
                    } else {
                        if (sellSideMap.empty()) {
                            //Dont have a matching order in Sell Side Insert To Buy Side
                            if (order.getTif() == 1 && order.getOrderType() == 1) {
                                //check the order is Day and Type is limit
                                mapIter = buySideMap.insert(priceOrderPair);
                                idIterPair = std::pair<string, MapIter>(order.getClientOrderID(),
                                                                        mapIter); // We store this in hashmap
                                buySideHashMap.insert(idIterPair); // Update the hashmap; order O(1)
                                string addMsg = "Added Order to Buy Side Order Book";
                                trading::OrderBook::SendExecutionReport(order, addMsg, '0', 0,
                                                                        order.getOrderQuantity(), 0, 0, "");
                                std::cout << trading::OrderBook::printBook() << std::endl;

                            } else {
                                // Rejected Order With TIF or because of Market Order
                                if (order.getTif() == 0) {
                                    string errMsg = "Rejected :: Cannot Insert IOC to Order Book";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(),
                                                                            "IOC Order Added ");

                                } else if (order.getOrderType() == 0) {
                                    string errMsg = "Rejected :: Cannot Insert Market Orders to Order Book";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(),
                                                                            "Market Order Added ");

                                } else {
                                    string errMsg = "Rejected :: Cannot Insert this Order ";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(), "Error in Order ");
                                }
                            }
                        } else {
                            //Try the Matching logic with Sell Side Order book
                            trading::OrderBook::getInstance().matchOrdersWithSellSide(order);
                        }
                    }
                    break;

                case EnumData::Sell:
                    if (!sellSideHashMap.empty() &&
                        ((hashMapIter = sellSideHashMap.find(order.getClientOrderID())) != sellSideHashMap.end())) {
                        //Error you cannot add duplicate client order ID's
                        //Error you cannot add duplicate client order ID's
                        string errMsg = "Rejected :: You cannot Add Duplicate Client Order ID's for the Order";
                        trading::OrderBook::SendExecutionReport(order, errMsg, '8', order.getTotalExecutedQuantity(),
                                                                order.getRemainingQuantity(), order.getExecutedPrice(),
                                                                order.getExecutedSize(),
                                                                "Duplicate Client ID's in the Order");
                    } else {
                        if (buySideMap.empty()) {
                            //Nothing to Match insert to Sell Side
                            if (order.getTif() == 1 && order.getOrderType() == 1) {
                                //check the order type is limit and the tif is date
                                mapIter = sellSideMap.insert(priceOrderPair);
                                idIterPair = std::pair<string, MapIter>(order.getClientOrderID(), mapIter);
                                sellSideHashMap.insert(idIterPair); // Update the hashmap; order O(1)

                                //send execution report for order added.
                                string addMsg = "Added Order to Sell Side Order Book";
                                trading::OrderBook::SendExecutionReport(order, addMsg, '0', 0,
                                                                        order.getRemainingQuantity(), 0, 0, "");
                                std::cout << trading::OrderBook::printBook() << std::endl;
                            } else {
                                // Rejected Order With TIF or because of Market Order
                                // Rejected Order With TIF or because of Market Order
                                if (order.getTif() == 0) {
                                    string errMsg = "Rejected :: Cannot Insert IOC to Order Book";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(),
                                                                            "IOC Order Added ");

                                } else if (order.getOrderType() == 0) {
                                    string errMsg = "Rejected :: Cannot Insert Market Orders to Order Book";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(),
                                                                            "Market Order Added ");

                                } else {
                                    string errMsg = "Rejected :: Cannot Insert this Order ";
                                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                                            order.getTotalExecutedQuantity(),
                                                                            order.getRemainingQuantity(),
                                                                            order.getExecutedPrice(),
                                                                            order.getExecutedSize(), "Error in Order ");
                                }
                            }
                        } else {
                            //Try the Matching logic
                            trading::OrderBook::getInstance().matchOrdersWithBuySide(order);
                        }
                    }


                    break;

                default:
                    throw BadOrderSide();
            }

            break;

        case EnumData::AMEND_ORDER: // update existing order
            //Yet to Implement (Better to keep Order Id in separate map for searching the value)
            if ((hashMapIter = buySideHashMap.find(order.getOriginalClientOrderID())) !=
                buySideHashMap.end()) {
                mapIter = hashMapIter->second;
                Order orderFromMap = mapIter->second;

                if ((orderFromMap.getPrice() != order.getPrice()) ||
                    (orderFromMap.getOrderQuantity() != order.getOrderQuantity())) {
                    orderFromMap.setOrderQuantity(order.getOrderQuantity());
                    orderFromMap.setPrice(order.getPrice());
                    orderFromMap.setOriginalClientOrderID(order.getOriginalClientOrderID());
                    orderFromMap.setClientOrderID(order.getClientOrderID());

                    orderFromMap.setExecutedPrice(0);
                    orderFromMap.setRemainingQuantity(order.getOrderQuantity());
                    orderFromMap.setExecType('5');
                    orderFromMap.setTotalExecutedQuantity(0);
                    orderFromMap.setExecutedSize(0);

                    buySideMap.erase(mapIter);
                    buySideHashMap.erase(hashMapIter);


                    mapIter = buySideMap.insert({orderFromMap.getPrice(), orderFromMap});
                    idIterPair = std::pair<string, MapIter>(orderFromMap.getClientOrderID(),
                                                            mapIter); // We store this in hashmap
                    buySideHashMap.insert(idIterPair); // Update the hashmap; order O(1)


                    string addMsg = "Amend Order to Buy Side Order Book";
                    trading::OrderBook::SendExecutionReport(orderFromMap, addMsg, orderFromMap.getExecType(), 0,
                                                            orderFromMap.getRemainingQuantity(), 0, 0, "Amend Order");
                    std::cout << trading::OrderBook::printBook() << std::endl;
                } else {
                    //Error in Modifying the Orders.
                    string errMsg = "Rejected :: You are only allow to modify the price and quantity of the orders ";
                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                            order.getTotalExecutedQuantity(),
                                                            order.getRemainingQuantity(),
                                                            order.getExecutedPrice(),
                                                            order.getExecutedSize(), "Error in Order Amendment ");

                }

            } else if ((hashMapIter = sellSideHashMap.find(order.getOriginalClientOrderID())) !=
                       sellSideHashMap.end()) { // working with asks

                mapIter = hashMapIter->second;
                Order orderFromMap = mapIter->second;
                if ((orderFromMap.getPrice() != order.getPrice()) ||
                    (orderFromMap.getOrderQuantity() != order.getOrderQuantity())) { // need to remove order completely;
                    sellSideMap.erase(mapIter);        // delete from map
                    sellSideHashMap.erase(hashMapIter);        // delete from hashmap
                    orderFromMap.setOrderQuantity(order.getOrderQuantity());
                    orderFromMap.setPrice(order.getPrice());
                    orderFromMap.setOriginalClientOrderID(order.getOriginalClientOrderID());
                    orderFromMap.setClientOrderID(order.getClientOrderID());

                    orderFromMap.setExecutedPrice(0);
                    orderFromMap.setRemainingQuantity(order.getOrderQuantity());
                    orderFromMap.setExecType('5');
                    orderFromMap.setTotalExecutedQuantity(0);
                    orderFromMap.setExecutedSize(0);

                    mapIter = sellSideMap.insert({orderFromMap.getPrice(), orderFromMap});
                    idIterPair = std::pair<string, MapIter>(orderFromMap.getClientOrderID(),
                                                            mapIter); // We store this in hashmap

                    sellSideHashMap.insert(idIterPair); // Update the hashmap; order O(1)

                    string addMsg = "Amend Order to Sell Side Order Book";
                    trading::OrderBook::SendExecutionReport(orderFromMap, addMsg, orderFromMap.getExecType(), 0,
                                                            orderFromMap.getRemainingQuantity(), 0, 0, "Amend Order");
                    std::cout << trading::OrderBook::printBook() << std::endl;
                } else { // need to update order
                    string errMsg = "Rejected :: You are only allow to modify the price and quantity of the orders ";
                    trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                            order.getTotalExecutedQuantity(),
                                                            order.getRemainingQuantity(),
                                                            order.getExecutedPrice(),
                                                            order.getExecutedSize(), "Error in Order Amendment ");
                }
            } else {
                string errMsg = "Rejected :: Attempt to Modify Non Exist Order";
                trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                        order.getTotalExecutedQuantity(),
                                                        order.getRemainingQuantity(),
                                                        order.getExecutedPrice(),
                                                        order.getExecutedSize(), "Error in Order Amendment ");

                throw AttempToReduceNonexistantOrder();
            }
            break;

        case EnumData::CANCEL_ORDER: //
            if ((hashMapIter = buySideHashMap.find(order.getOriginalClientOrderID())) !=
                buySideHashMap.end()) { // working with bids
                mapIter = hashMapIter->second;
                buySideMap.erase(mapIter);
                buySideHashMap.erase(hashMapIter);


                string addMsg = "Cancel Order from Buy Side Order Book";
                trading::OrderBook::SendExecutionReport(order, addMsg, '4', 0,
                                                        order.getRemainingQuantity(), 0, 0, "Cancel Order");
                std::cout << trading::OrderBook::printBook() << std::endl;

            } else if ((hashMapIter = sellSideHashMap.find(order.getOriginalClientOrderID())) !=
                       sellSideHashMap.end()) {
                mapIter = hashMapIter->second;
                sellSideMap.erase(mapIter);        // delete from map
                sellSideHashMap.erase(hashMapIter);

                string addMsg = "Cancel Order from Sell Side Order Book";
                trading::OrderBook::SendExecutionReport(order, addMsg, '4', 0,
                                                        order.getRemainingQuantity(), 0, 0, "Cancel Order");
                std::cout << trading::OrderBook::printBook() << std::endl;
            } else {
                //Send Rejection execution Report
                string errMsg = "Rejected :: Attempt to Cancel Non Exist Order";
                trading::OrderBook::SendExecutionReport(order, errMsg, '8',
                                                        order.getTotalExecutedQuantity(),
                                                        order.getRemainingQuantity(),
                                                        order.getExecutedPrice(),
                                                        order.getExecutedSize(), "Error in Order Cancel ");
            }
            break;

        default:
            std::cout << "Inside ProcessOrder -> Default Error " << std::endl;
            throw BadOrderType(); // should never be here
    }

}


inline void trading::OrderBook::matchOrdersWithSellSide(Order &buySideOrder) {
    // Map Iterator
    MapIter mapIter; // for the return value when inserting into multimap

    // Pair for the hashmap
    std::pair<string, MapIter> idIterPair;
    HashMapIter hashMapIter;
//    std::pair<HashMapIter, bool> hashRet; // for the return value when inserting into hashmap


    for (MapIter it = sellSideMap.begin(); it != sellSideMap.end();) {
        Order sellSideOrder = it->second;
        if ((buySideOrder.getOrderType() == 1) && (buySideOrder.getTif() == 1) &&
            (buySideOrder.getPrice() < sellSideOrder.getPrice())) {
            //Unable to Match with Sell Side Added to Order book
            mapIter = buySideMap.insert({buySideOrder.getPrice(), buySideOrder});
            idIterPair = std::pair<string, MapIter>(buySideOrder.getClientOrderID(), mapIter);
            buySideHashMap.insert(idIterPair); // Update the hashmap; order O(1)

            string addMsg = "Added Order to Buy Side Order Book";
            trading::OrderBook::SendExecutionReport(buySideOrder, addMsg, '0', 0,
                                                    buySideOrder.getOrderQuantity(), 0, 0, "");
            std::cout << trading::OrderBook::printBook() << std::endl;
            break;
        } else {
            //Can Be Market Order or IOC as well
            //Sell Side item price is less than buy order price can trade
            if (buySideOrder.getTif() == 2 && buySideOrder.getPrice() < sellSideOrder.getPrice()) {
                //Do not need to Add it to the Order Book Send Expired Execution Report.
                string expMsg = "Expired :: Remaining IOC Order Quantity Expired ";
                trading::OrderBook::SendExecutionReport(buySideOrder, expMsg, 'C',
                                                        buySideOrder.getTotalExecutedQuantity(),
                                                        buySideOrder.getRemainingQuantity(),
                                                        buySideOrder.getExecutedPrice(),
                                                        buySideOrder.getExecutedSize(),
                                                        "IOC Order Expired ");
                break;
            } else {

                if (sellSideOrder.getRemainingQuantity() > buySideOrder.getRemainingQuantity()) {
                    std::stringstream oss;

                    //buy side order traded completely. remain quantity will be update on sell side map
                    sellSideOrder.setRemainingQuantity(
                            sellSideOrder.getRemainingQuantity() - buySideOrder.getRemainingQuantity());
                    it->second = sellSideOrder;

                    //send execution reports for sell side order.
                    double executedPrice = ((buySideOrder.getOrderType() == 1) ? buySideOrder.getPrice()
                                                                               : sellSideOrder.getPrice());
                    oss << buySideOrder.getRemainingQuantity() << " get executed at " << executedPrice
                        << " of the previous BUY SIDE ORDER " << std::endl;
                    string excSellMsg = oss.str();
                    trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                            (sellSideOrder.getTotalExecutedQuantity() +
                                                             buySideOrder.getRemainingQuantity()),
                                                            sellSideOrder.getRemainingQuantity(), executedPrice,
                                                            buySideOrder.getRemainingQuantity(),
                                                            " Partially Traded With Buy Order ");

                    //send execution reports for buy side order.
                    oss.clear();
                    oss.str(std::string());
                    oss << buySideOrder.getRemainingQuantity() << " get executed at " << executedPrice
                        << " of the previous SELL SIDE ORDER " << std::endl;
                    string excBuyMsg = oss.str();
                    trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                            (buySideOrder.getTotalExecutedQuantity() +
                                                             buySideOrder.getRemainingQuantity()), 0,
                                                            executedPrice, buySideOrder.getRemainingQuantity(),
                                                            "Fully Traded With Sell Side Order");


                    std::cout << trading::OrderBook::printBook() << std::endl;

                    break;
                } else {
                    //Buy Side Order Quantity is greater than sell side Quantity.(sell side traded completely)
                    double remainBuyOrderQuantity =
                            buySideOrder.getRemainingQuantity() - sellSideOrder.getRemainingQuantity();
                    if (remainBuyOrderQuantity > 0) {
                        //Check for the next sell match and store the remaining qty in buy map. erase the traded value from sell side
                        buySideOrder.setRemainingQuantity(remainBuyOrderQuantity);
                        std::stringstream oss;
                        //Send Trade Report to Sell User and Buy User
                        MapIter save = it;
                        save++;
                        sellSideHashMap.erase(sellSideHashMap.find(it->second.getClientOrderID()));
                        sellSideMap.erase(it);

                        double executedPrice = ((buySideOrder.getOrderType() == 1) ? buySideOrder.getPrice()
                                                                                   : sellSideOrder.getPrice());

                        //send execution reports for buy side order.
                        oss << sellSideOrder.getRemainingQuantity() << " get executed and partially traded at "
                            << executedPrice << " of the previous SELL SIDE ORDER " << std::endl;
                        string excBuyMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                                (buySideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()),
                                                                buySideOrder.getRemainingQuantity(),
                                                                executedPrice,
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Partially Traded with Sell Order");

                        //send execution reports for sell side order.
                        oss.clear();//clear
                        oss.str(std::string());
                        oss << sellSideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << executedPrice << " of the previous BUY SIDE ORDER " << std::endl;
                        string excSellMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                                (sellSideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()), 0,
                                                                executedPrice,
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Fully Traded the Quantity with buy Order");

                        std::cout << trading::OrderBook::printBook() << std::endl;


                        if (sellSideMap.size() > 0) {
                            it = save;
                        } else {
                            //When we haven't any thing in sell side to trade
                            if (buySideOrder.getOrderType() == 2) {
                                //Do not want to update the Remaining Quantity to Order book.
                                string expMsg = "Expired :: Remaining Market Data Order Quantity Expired ";
                                trading::OrderBook::SendExecutionReport(buySideOrder, expMsg, 'C',
                                                                        buySideOrder.getTotalExecutedQuantity(),
                                                                        buySideOrder.getRemainingQuantity(),
                                                                        buySideOrder.getExecutedPrice(),
                                                                        buySideOrder.getExecutedSize(),
                                                                        "Market Data Order Expired ");
                                break;
                            } else if (buySideOrder.getTif() == 2) {
                                string expMsg = "Expired :: Remaining IOC Order Quantity Expired ";
                                trading::OrderBook::SendExecutionReport(buySideOrder, expMsg, 'C',
                                                                        buySideOrder.getTotalExecutedQuantity(),
                                                                        buySideOrder.getRemainingQuantity(),
                                                                        buySideOrder.getExecutedPrice(),
                                                                        buySideOrder.getExecutedSize(),
                                                                        "IOC Order Expired ");
                                break;
                            }
                            buySideMap.insert({buySideOrder.getPrice(), buySideOrder});
                            idIterPair = std::pair<string, MapIter>(buySideOrder.getClientOrderID(),
                                                                    mapIter); // We store this in hashmap
                            buySideHashMap.insert(idIterPair); // Update the hashmap; order O(1)

                            //Send Execution Report For Buy Side user Regarding Order book update.
                            string addMsg = "Added Order to Buy Side Order Book";
                            trading::OrderBook::SendExecutionReport(buySideOrder, addMsg, '0', 0,
                                                                    buySideOrder.getRemainingQuantity(), 0, 0, "");
                            std::cout << trading::OrderBook::printBook() << std::endl;
                            break;

                        }

                    } else {
                        std::stringstream oss;
                        //Need to Send Execution Report for the Trade
                        //Buy Side should be equal Sell Side map have to remove from both buy side and sell side
                        sellSideHashMap.erase(sellSideHashMap.find(it->second.getClientOrderID()));
                        sellSideMap.erase(it);

                        oss << sellSideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << buySideOrder.getPrice() << " of the previous SELL SIDE ORDER " << std::endl;
                        string excBuyMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                                (buySideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()), 0,
                                                                buySideOrder.getPrice(),
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Fully Traded With the Sell Side Order");

                        //send execution reports for sell side order.
                        oss << sellSideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << buySideOrder.getPrice() << " of the previous BUY SIDE ORDER " << std::endl;
                        string excSellMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                                (sellSideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()), 0,
                                                                buySideOrder.getPrice(),
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Fully Traded the Quantity with buy Order");

                        std::cout << trading::OrderBook::printBook() << std::endl;

                        break;
                    }

                    //Trade Complete and Remove from Sell Side
                }
            }
        }
    }

}


inline void trading::OrderBook::matchOrdersWithBuySide(Order &sellSideOrder) {

    // Map Iterator
    MapIter mapIter; // for the return value when inserting into multimap

    // Pair for the hashmap
    std::pair<string, MapIter> idIterPair;
    HashMapIter hashMapIter;
//    std::pair<HashMapIter, bool> hashRet; // for the return value when inserting into hashmap

    for (MapIter it = buySideMap.begin(); it != buySideMap.end();) {
        Order buySideOrder = it->second;
        if ((sellSideOrder.getOrderType() == 1) && (sellSideOrder.getTif() == 1) &&
            buySideOrder.getPrice() < sellSideOrder.getPrice()) {
            //Cant Trade added to sell Side
            mapIter = sellSideMap.insert({sellSideOrder.getPrice(), sellSideOrder});
            // We store this in hashmap
            idIterPair = std::pair<string, MapIter>(sellSideOrder.getClientOrderID(), mapIter);
            sellSideHashMap.insert(idIterPair); // Update the hashmap; order O(1)
            //send execution report for order added.
            string addMsg = "Added Order to Sell Side Order Book";
            trading::OrderBook::SendExecutionReport(sellSideOrder, addMsg, '0', 0,
                                                    sellSideOrder.getRemainingQuantity(), 0, 0, "");
            std::cout << trading::OrderBook::printBook() << std::endl;
            break;

        } else {
            if (sellSideOrder.getTif() == 2 && buySideOrder.getPrice() < sellSideOrder.getPrice()) {
                //Do not need to Add it to the Order Book Send Expired Execution Report.
                string expMsg = "Expired :: Remaining IOC Order Quantity Expired ";
                trading::OrderBook::SendExecutionReport(sellSideOrder, expMsg, 'C',
                                                        sellSideOrder.getTotalExecutedQuantity(),
                                                        sellSideOrder.getRemainingQuantity(),
                                                        sellSideOrder.getExecutedPrice(),
                                                        sellSideOrder.getExecutedSize(),
                                                        "IOC Order Expired ");
                break;
            } else {

                //Buy side order price is greater than sell side order price
                if (buySideOrder.getRemainingQuantity() > sellSideOrder.getRemainingQuantity()) {
                    std::stringstream oss;
                    //Sell Side Order Will be traded completely
                    buySideOrder.setRemainingQuantity(
                            buySideOrder.getRemainingQuantity() - sellSideOrder.getRemainingQuantity());
                    it->second = buySideOrder;
                    //Send Excecution Reports for both Sell side and Buy Side.


                    //send execution reports for buy side order.
                    oss << sellSideOrder.getRemainingQuantity() << " get executed at " << buySideOrder.getPrice()
                        << " of the previous SELL SIDE ORDER " << std::endl;
                    string excBuyMsg = oss.str();
                    trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                            (buySideOrder.getTotalExecutedQuantity() +
                                                             buySideOrder.getRemainingQuantity()),
                                                            buySideOrder.getRemainingQuantity(),
                                                            buySideOrder.getPrice(),
                                                            sellSideOrder.getRemainingQuantity(),
                                                            "Partially Traded With Sell Side Order");

                    //send execution reports for sell side order.
                    oss.clear();
                    oss.str(std::string());
                    oss << sellSideOrder.getRemainingQuantity() << " get executed at " << buySideOrder.getPrice()
                        << " of the previous BUY SIDE ORDER " << std::endl;
                    string excSellMsg = oss.str();
                    trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                            (sellSideOrder.getTotalExecutedQuantity() +
                                                             sellSideOrder.getRemainingQuantity()),
                                                            0, buySideOrder.getPrice(),
                                                            sellSideOrder.getRemainingQuantity(),
                                                            " Fully Traded With Buy Order ");

                    std::cout << trading::OrderBook::printBook() << std::endl;
                    break;
                } else {
                    std::stringstream oss;
                    //Sell Order Quantity is Greater than buy side order
                    double remainSellSideOrderQuantity =
                            sellSideOrder.getRemainingQuantity() - buySideOrder.getRemainingQuantity();
                    if (remainSellSideOrderQuantity > 0) {
                        sellSideOrder.setRemainingQuantity(remainSellSideOrderQuantity);
                        MapIter save = it;
                        save++;
                        buySideHashMap.erase(buySideHashMap.find(it->second.getClientOrderID()));
                        buySideMap.erase(it);


                        //send execution reports for sell side order.
                        oss << buySideOrder.getRemainingQuantity() << " get executed and partially traded at "
                            << buySideOrder.getPrice() << " of the previous BUY SIDE ORDER " << std::endl;
                        string excSellMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                                (sellSideOrder.getTotalExecutedQuantity() +
                                                                 buySideOrder.getRemainingQuantity()),
                                                                sellSideOrder.getRemainingQuantity(),
                                                                buySideOrder.getPrice(),
                                                                buySideOrder.getRemainingQuantity(),
                                                                "Partially Traded with buy Order");

                        oss.clear();//clear
                        oss.str(std::string());
                        //send execution reports for buy side order.
                        oss << buySideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << buySideOrder.getPrice() << " of the previous SELL SIDE ORDER " << std::endl;
                        string excBuyMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                                (buySideOrder.getTotalExecutedQuantity() +
                                                                 buySideOrder.getRemainingQuantity()),
                                                                0, buySideOrder.getPrice(),
                                                                buySideOrder.getRemainingQuantity(),
                                                                "Fully Traded with Sell Order");

                        std::cout << trading::OrderBook::printBook() << std::endl;

                        if (buySideMap.size() > 0) {
                            it = save;
                        } else {
                            //When we haven't any thing in sell side to trade
                            //When we haven't any thing in sell side to trade
                            if (sellSideOrder.getOrderType() == 2) {
                                //Do not want to update the Remaining Quantity to Order book.
                                string expMsg = "Expired :: Remaining Market Data Order Quantity Expired ";
                                trading::OrderBook::SendExecutionReport(sellSideOrder, expMsg, 'C',
                                                                        sellSideOrder.getTotalExecutedQuantity(),
                                                                        sellSideOrder.getRemainingQuantity(),
                                                                        sellSideOrder.getExecutedPrice(),
                                                                        sellSideOrder.getExecutedSize(),
                                                                        "Market Data Order Expired ");
                                break;
                            } else if (sellSideOrder.getTif() == 2) {
                                string expMsg = "Expired :: Remaining IOC Order Quantity Expired ";
                                trading::OrderBook::SendExecutionReport(sellSideOrder, expMsg, 'C',
                                                                        sellSideOrder.getTotalExecutedQuantity(),
                                                                        sellSideOrder.getRemainingQuantity(),
                                                                        sellSideOrder.getExecutedPrice(),
                                                                        sellSideOrder.getExecutedSize(),
                                                                        "IOC Order Expired ");
                                break;
                            }
                            //When we haven't any thing in buy side to match
                            mapIter = sellSideMap.insert({sellSideOrder.getPrice(), sellSideOrder});
                            // We store this in hashmap
                            idIterPair = std::pair<string, MapIter>(sellSideOrder.getClientOrderID(), mapIter);
                            sellSideHashMap.insert(idIterPair); // Update the hashmap; order O(1)


                            //Send Execution Report For Buy Side user Regarding Order book update.
                            string addMsg = "Added Order to Sell Side Order Book";
                            trading::OrderBook::SendExecutionReport(sellSideOrder, addMsg, '0', 0,
                                                                    sellSideOrder.getRemainingQuantity(), 0, 0, "");
                            std::cout << trading::OrderBook::printBook() << std::endl;
                            break;
                        }

                    } else {
                        //Need to Send Execution Report for the Trade
                        //Buy Side should be equal Sell Side map have to remove from both buy side and sell side
                        oss.clear();//clear
                        oss.str(std::string());
                        //Send Trade Report to Sell User and Buy User
                        buySideHashMap.erase(buySideHashMap.find(it->second.getClientOrderID()));
                        buySideMap.erase(it);

                        oss << sellSideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << buySideOrder.getPrice() << " of the previous SELL SIDE ORDER " << std::endl;
                        string excBuyMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(buySideOrder, excBuyMsg, 'F',
                                                                (buySideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()), 0,
                                                                buySideOrder.getPrice(),
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Fully Traded With the Sell Side Order");
                        oss.clear();//clear
                        oss.str(std::string());
                        //send execution reports for sell side order.
                        oss << sellSideOrder.getRemainingQuantity() << " get executed and fully traded at "
                            << buySideOrder.getPrice() << " of the previous BUY SIDE ORDER " << std::endl;
                        string excSellMsg = oss.str();
                        trading::OrderBook::SendExecutionReport(sellSideOrder, excSellMsg, 'F',
                                                                (sellSideOrder.getTotalExecutedQuantity() +
                                                                 sellSideOrder.getRemainingQuantity()), 0,
                                                                buySideOrder.getPrice(),
                                                                sellSideOrder.getRemainingQuantity(),
                                                                "Fully Traded with the  buy Order");

                        std::cout << trading::OrderBook::printBook() << std::endl;

                        break;
                    }
                }
            }
        }
    }
}



//#endif //RANGANEW_ORDERBOOK_H
