//#ifndef ORDER_H_
//#define ORDER_H_
#pragma once

#include <string>
#include <sstream>

using namespace std;

class Order {


public:
    std::string toString(Order &order) const;

    const string getSymbol() const;

    void setSymbol(const string m_symbol);

    int getSide() const;

    void setSide(int m_side);

    int getOrderType() const;

    void setOrderType(int m_orderType);

    int getTif() const;

    void setTif(int m_tif);

    double getOrderQuantity() const;

    void setOrderQuantity(double m_orderQuantity);

    double getPrice() const;

    void setPrice(double m_price);

    const string getUserID() const;

    void setUserID(const string m_userID);

    const string getClientOrderID() const;

    void setClientOrderID(const string m_clientOrderID);

    const string getOriginalClientOrderID() const;

    void setOriginalClientOrderID(const string m_originalClientOrderID);

    unsigned long getTimestamp() const;

    void setTimestamp(unsigned long m_timestamp);

    void setMsgType(int msgType);

    int getMsgType();

    double getTotalExecutedQuantity() const;

    void setTotalExecutedQuantity(double m_executedQuantity);

    double getRemainingQuantity() const;

    void setRemainingQuantity(double m_remainingQuantity);

    const char &getExecType() const;

    void setExecType(const char &m_execType);

    double getExecutedSize() const;

    void setExecutedSize(double m_executedSize);

    double getExecutedPrice() const;

    void setExecutedPrice(double m_executedPrice);

    const string *getReason() const;

    void setReason(const string *m_reason);

private:
    string m_symbol;
    int m_side;
    int m_orderType;
    int m_tif;
    double m_orderQuantity;
    double m_price;
    string m_userID;
    string m_clientOrderID;
    string m_originalClientOrderID;
    unsigned long int m_timestamp;
    int m_msgType;


//Execution Report
    double m_totalExecutedQuantity;
    double m_remainingQuantity;
    char m_execType;
    double m_executedSize;
    double m_executedPrice;
    const string *m_reason;

};

inline std::string Order::toString(Order &order) const {
    std::stringstream oss;

    if (order.getMsgType() == 1) {
        oss << "AddOrder: " << order.getTimestamp() << " " << order.getClientOrderID() << " "
            << (order.getSide() == 1 ? "B" : "S") <<
            " " << static_cast<double>(order.getPrice())  << " " << order.getOrderQuantity();
    } else {
        oss << "ReduceOrder: " << order.getTimestamp() << " " << order.getClientOrderID() << " "
            << order.getOrderQuantity();
    }
    return oss.str();
}
//#endif /* ORDER_H_ */