#ifndef STATEEVENT_H_
#define STATEEVENT_H_

// System includes
#include <QEvent>
#include <QVariant>

// User includes

/**
 * Custom event type used for controlling the image engine thread.
 */
class GenStateEvent : public QEvent
    {
public:  
    //! Application manager state enum
    enum AppManagerState
    {
        StateUninitialized = 0,
        StateInitialized,
        StateIdle,
        StateListDropBox,
        StateDetails
    };

    static const QEvent::Type GenStateEventType =
            static_cast<QEvent::Type> (QEvent::User + 666);

    GenStateEvent(AppManagerState state) :
        QEvent(GenStateEventType), _state(state)
        {

        }

    Type type() const
        {
        return GenStateEventType;
        }

    AppManagerState state() const
        {
        return _state;
        }

private:
    AppManagerState _state;
    };

#endif /* STATEEVENT_H_ */ 
