#pragma once
#include "../Logger/Logger.h"
#include <map>
#include <typeindex>
#include <memory>
#include <list>

class Event
{
public:
    Event() = default;
};

class IEventCallback
{
private:
    virtual void Call(Event& e) =0;
public:
    virtual ~IEventCallback() = default;
    void Execute(Event &e)
    {
        Call(e);
    }
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
    private:
        typedef void (TOwner::*CallbackFunction)(TEvent&);
        TOwner* ownerInstance;
        CallbackFunction callbackFunction;

    virtual void Call(Event& e) override
    {
        std::invoke(callbackFunction, ownerInstance, static_cast<TEvent>(e));
    }

    public:
        EventCallback(TOwner* ownerInstance, CallbackFunction callbackFunction)
        {
            this->ownerInstance = ownerInstance;
            this->callbackFunction = callbackFunction;
        }

        virtual ~EventCallback() overrid = default;
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus
{
private:
    std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
    EventBus()
    {
        Logger::Log("Event bus constructed.");
    }

    ~EventBus()
    {
        Logger::Log("Event bus destroyed.");
    }

template<typename TEvent>
    void EmitEvent()
    {
        auto handlers = subscribers[typeid(TEvent)].get(); 
    }

    template<typename TEvent, typename TOwner>
    void SubscribeToEvent(TOwner* ownerInstance, void(TOwner::*callbackFunction)(TEvent&))
    {
        if(subscribers[typeid(TEvent)].get() == nullptr)
        {
            subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
        }
        auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunction);
        subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
    }
};