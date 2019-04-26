//
// Created by wijekoonw on 4/18/2019.
//

#ifndef RANGANEW_ENUMDATA_H
#define RANGANEW_ENUMDATA_H


class EnumData {
public:
    enum OrderSide {
        Buy = 1, Sell = 2
    };
public:
    enum MsgType {
        NEW_ORDER = 1, AMEND_ORDER = 2, CANCEL_ORDER = 3
    };

    enum ExecTypeEnum {
        New = '0',
        Executed = 'F',
        Amend = '5',
        Expired = 'C',
        Cancel = '4',
        Rejected = '8'
    };
};




#endif //RANGANEW_ENUMDATA_H
