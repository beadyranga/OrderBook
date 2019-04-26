//
// Created by wijekoonw on 4/11/2019.
//

#include <iostream>
#include "OrderBook.h"


trading::OrderBook::BidsMap trading::OrderBook::buySideMap = trading::OrderBook::BidsMap();
trading::OrderBook::AsksMap trading::OrderBook::sellSideMap = trading::OrderBook::AsksMap();

trading::OrderBook::BidsHash trading::OrderBook::buySideHashMap = trading::OrderBook::BidsHash();
trading::OrderBook::AsksHash trading::OrderBook::sellSideHashMap = trading::OrderBook::AsksHash();

void trading::OrderBook::SendExecutionReport(Order &order,string execMsg, char exeType, double totalExcQuantity,
                                              double remainingQuantity, double execPrice, double execSize,
                                              string reason) {
    std::stringstream oss;
    order.setExecType(exeType);
    order.setTotalExecutedQuantity(totalExcQuantity);
    order.setRemainingQuantity(remainingQuantity);
    order.setExecutedPrice(execPrice);
    order.setExecutedSize(execSize);
    order.setReason(&reason);


    oss << std::endl;
    std::cout << "=======================Execution Report Send============================================================" << std::endl;
    std::cout << execMsg << std::endl;
    oss << "ORD_TYPE\tTIF\tPRICE\tEC_TYPE\tORD_QTY\tTO_EX_QTY\tREM_QTY\tEXE_SIZE\tEXE_PRICE\tREASON" << std::endl;
    oss <<((order.getOrderType() == 1) ? "LIMIT" : "MARKET")<<"\t\t" << ((order.getTif() == 1) ? "DAY" : "IOC") <<"\t"
        << order.getPrice() << "\t" << order.getExecType() << "\t"<< order.getOrderQuantity() << "\t"
        << order.getTotalExecutedQuantity()<<"\t\t"<<order.getRemainingQuantity()<<"\t"<<order.getExecutedSize()<<"\t\t"
        << order.getExecutedPrice()<<"\t\t"<<*order.getReason();
    std::cout << oss.str() << std::endl;
    std::cout << "========================================================================================================" << std::endl;
//    switch (exeType) {
//        case EnumData::New:
////            << ((order.getMsgType() == 1) ? ((order.getMsgType() == 2) ? "Amend" : "Cancel") : "New") <<"/t"
//
//            break;
//        case EnumData::Amend:
//            oss << ((order.getMsgType() == 1) ? ((order.getMsgType() == 2) ? "Amend" : "Cancel") : "New") <<
//                ((order.getOrderType() == 1) ? "LIMIT" : "MARKET") << ((order.getTif() == 1) ? "DAY" : "IOC")
//                << "order " <<
//                order.getOrderQuantity() << "@" << order.getPrice() << "Added to the Order BooK"
//                << (order.getSide() == 1) ? "Sell Side" : "Buy Side" ;
//            std::cout << oss.str() << std::endl;
//            break;
//        case EnumData::Executed:
//            break;
//        case EnumData::Cancel:
//            break;
//        case EnumData::Rejected:
//            break;
//        case EnumData::Expired:
//            break;
//    }

    //obtq->Add(order);
}

std::string trading::OrderBook::printBook() {
    std::stringstream oss;
    Order order;

    oss << std::endl;
    oss << "Order book:" << std::endl;
    oss << "BUY SIDE\t\tSELL SIDE" << std::endl;


    for (MapReverseIter it = sellSideMap.rbegin(); it != sellSideMap.rend(); it++) {
        order = it->second;
        oss << "\t\t\t" << static_cast<double>(order.getPrice()) << " x " << order.getRemainingQuantity() << "\t// "
            << order.toString(order)
            << std::endl;
    }

    for (MapIter it = buySideMap.begin(); it != buySideMap.end(); it++) {
        order = it->second;
        oss << "" << static_cast<double>(order.getPrice()) << " x " << order.getRemainingQuantity() << "\t\t\t\t// "
            << order.toString(order)
            << std::endl;
    }

    oss << "======================================================================================" << std::endl;

    return oss.str();
}

