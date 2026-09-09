//
// Created by fog54 on 09.09.2026.
//

#ifndef QMI_MODEM_MODEMSTATE_H
#define QMI_MODEM_MODEMSTATE_H


enum  class ModemState {
    Off,
    Booting,
    Ready,
    SearchingNetwork,
    Registered,
    DataConnected
};


#endif //QMI_MODEM_MODEMSTATE_H
