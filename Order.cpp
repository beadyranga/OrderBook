#include "Order.h"

void Order::setSymbol(const string m_symbol) {
	Order::m_symbol = m_symbol;
}

const string Order::getSymbol() const {
    return m_symbol;
}


int Order::getSide() const {
	return m_side;
}

void Order::setSide(int m_side) {
	Order::m_side = m_side;
}

int Order::getOrderType() const {
	return m_orderType;
}

void Order::setOrderType(int m_orderType) {
	Order::m_orderType = m_orderType;
}

int Order::getTif() const {
	return m_tif;
}

void Order::setTif(int m_tif) {
	Order::m_tif = m_tif;
}

double Order::getOrderQuantity() const {
	return m_orderQuantity;
}

void Order::setOrderQuantity(double m_orderQuantity) {
	Order::m_orderQuantity = m_orderQuantity;
}

double Order::getPrice() const {
	return m_price;
}

void Order::setPrice(double m_price) {
	Order::m_price = m_price;
}

const string Order::getUserID() const {
	return m_userID;
}

void Order::setUserID(const string m_userID) {
	Order::m_userID = m_userID;
}

const string Order::getClientOrderID() const {
	return m_clientOrderID;
}

void Order::setClientOrderID(const string m_clientOrderID) {
	Order::m_clientOrderID = m_clientOrderID;
}

const string Order::getOriginalClientOrderID() const {
	return m_originalClientOrderID;
}

void Order::setOriginalClientOrderID(const string m_originalClientOrderID) {
	Order::m_originalClientOrderID = m_originalClientOrderID;
}

unsigned long Order::getTimestamp() const {
	return m_timestamp;
}

void Order::setTimestamp(unsigned long m_timestamp) {
	Order::m_timestamp = m_timestamp;
}

void Order::setMsgType(int msgType) {
	m_msgType = msgType;
}

int Order::getMsgType() {
	return m_msgType;
}

double Order::getTotalExecutedQuantity() const {
	return m_totalExecutedQuantity;
}

void Order::setTotalExecutedQuantity(double m_executedQuantity) {
	Order::m_totalExecutedQuantity = m_executedQuantity;
}

double Order::getRemainingQuantity() const {
	return m_remainingQuantity;
}

void Order::setRemainingQuantity(double m_remainingQuantity) {
	Order::m_remainingQuantity = m_remainingQuantity;
}

const char &Order::getExecType() const {
	return m_execType;
}

void Order::setExecType(const char &m_execType) {
	Order::m_execType = m_execType;
}

double Order::getExecutedSize() const {
	return m_executedSize;
}

void Order::setExecutedSize(double m_executedSize) {
	Order::m_executedSize = m_executedSize;
}

double Order::getExecutedPrice() const {
	return m_executedPrice;
}

void Order::setExecutedPrice(double m_executedPrice) {
	Order::m_executedPrice = m_executedPrice;
}

const string *Order::getReason() const {
	return m_reason;
}

void Order::setReason(const string *m_reason) {
	Order::m_reason = m_reason;
}

