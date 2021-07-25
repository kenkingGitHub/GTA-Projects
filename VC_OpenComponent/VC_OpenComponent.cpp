/*
Plugin-SDK (Grand Theft Auto) source file
Authors: GTA Community. See more here
https://github.com/DK22Pac/plugin-sdk
Do not delete this comment block. Respect others' work!
*/
#include <plugin_vc.h>
#include "game_vc\common.h"
#include "game_vc\CAutomobile.h"
#include "game_vc\CTimer.h"

using namespace plugin;

const float ACTION_TIME_STEP = 0.05f;
const unsigned int TIME_FOR_KEYPRESS = 500;

class DoorsExample {
public:
    static int componentByDoorId[6]; // Таблица перевода eDoors в Id компонента

    static int m_nLastTimeWhenAnyActionWasEnabled; // Последнее время запуска события

    enum eDoorEventType { // Тип события
        DOOR_EVENT_OPEN,
        DOOR_EVENT_CLOSE
    };

    class DoorEvent { // Класс события
    public:
        bool m_active;
        eDoorEventType m_type;
        float m_openingState;

        DoorEvent() {
            m_active = false;
            m_type = DOOR_EVENT_CLOSE;
        }
    };

    class VehicleDoors {
    public:
        DoorEvent events[6]; // События для всех 6 дверей

        VehicleDoors(CVehicle *) {}
    };

    static VehicleExtendedData<VehicleDoors> VehDoors; // Наше расширение

    static void EnableDoorEvent(CAutomobile *automobile, eDoors doorId) { // Включить событие двери
        if (automobile->IsComponentPresent(componentByDoorId[doorId])) {
            if (automobile->m_carDamage.GetDoorStatus(doorId) != DAMSTATE_NOTPRESENT) {
                DoorEvent &event = VehDoors.Get(automobile).events[doorId];
                if (event.m_type == DOOR_EVENT_OPEN)
                    event.m_type = DOOR_EVENT_CLOSE; // Если последнее событие - открытие, то закрываем
                else
                    event.m_type = DOOR_EVENT_OPEN; // Если последнее событие закрытие - то открываем
                event.m_active = true; // Включаем обработку
                m_nLastTimeWhenAnyActionWasEnabled = CTimer::m_snTimeInMilliseconds;
            }
        }
    }

    static void ProcessDoors(CVehicle *vehicle) { // Обработка событий для конкретного авто
        if (vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
            CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle);
            for (unsigned int i = 0; i < 6; i++) { // Обрабатываем все события
                eDoors doorId = static_cast<eDoors>(i);
                DoorEvent &event = VehDoors.Get(automobile).events[doorId];
                if (event.m_active) { // Если событие активно
                    if (event.m_type == DOOR_EVENT_OPEN) {
                        event.m_openingState += ACTION_TIME_STEP;
                        if (event.m_openingState > 1.0f) { // Если полностью открыли
                            event.m_active = false; // Отключаем обработку
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, 1.0f); // Полностью открываем
                            event.m_openingState = 1.0f;
                        }
                        else
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, event.m_openingState);
                    }
                    else {
                        event.m_openingState -= ACTION_TIME_STEP;
                        if (event.m_openingState < 0.0f) { // Если полностью открыли
                            event.m_active = false; // Отключаем обработку
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, 0.0f); // Полностью открываем
                            event.m_openingState = 0.0f;
                        }
                        else
                            automobile->OpenDoor(componentByDoorId[doorId], doorId, event.m_openingState);
                    }
                }
            }
        }
    }

    static void MainProcess() { // Обработка нажатия клавиш и запуск событий
        if (CTimer::m_snTimeInMilliseconds > (m_nLastTimeWhenAnyActionWasEnabled + TIME_FOR_KEYPRESS)) { // если прошло 500 мс с того времени, как мы начали открывать/закрывать что-то
            CVehicle *vehicle = FindPlayerVehicle();
            if (vehicle && vehicle->m_nVehicleClass == VEHICLE_AUTOMOBILE) {
                CAutomobile *automobile = reinterpret_cast<CAutomobile *>(vehicle); // опять же, приведение типов. Т.к. мы будет юзать damageManager, нам нужно убедиться, что транспорт - это автомобиль (CAutomobile)
                if (KeyPressed(219)) // [
                    EnableDoorEvent(automobile, BONNET); // капот
                else if (KeyPressed(221)) // ]
                    EnableDoorEvent(automobile, BOOT); // багажник
                else if (KeyPressed(186) && KeyPressed(187)) // ; =
                    EnableDoorEvent(automobile, DOOR_FRONT_LEFT); // левая передняя дверь
                else if (KeyPressed(222) && KeyPressed(187)) // ' =
                    EnableDoorEvent(automobile, DOOR_FRONT_RIGHT); // правая передняя дверь
                else if (KeyPressed(186) && KeyPressed(189)) // ; -
                    EnableDoorEvent(automobile, DOOR_REAR_LEFT); // левая задняя дверь
                else if (KeyPressed(222) && KeyPressed(189)) // ' -
                    EnableDoorEvent(automobile, DOOR_REAR_RIGHT); // правая задняя дверь
                else if (KeyPressed(VK_F12)) {
                    EnableDoorEvent(automobile, BONNET);
                    EnableDoorEvent(automobile, BOOT);
                    EnableDoorEvent(automobile, DOOR_FRONT_LEFT);
                    EnableDoorEvent(automobile, DOOR_FRONT_RIGHT);
                    EnableDoorEvent(automobile, DOOR_REAR_LEFT);
                    EnableDoorEvent(automobile, DOOR_REAR_RIGHT);
                }
            }
        }
    }

    DoorsExample() {
        Events::gameProcessEvent += MainProcess; // Тут обрабатываем нажатия и запускаем события
        Events::vehicleRenderEvent += ProcessDoors; // Тут обрабатываем события, а также выключаем их
    }
} example;

int DoorsExample::componentByDoorId[6] = { CAR_BONNET, CAR_BOOT, CAR_DOOR_LF, CAR_DOOR_RF, CAR_DOOR_LR, CAR_DOOR_RR };
int DoorsExample::m_nLastTimeWhenAnyActionWasEnabled = 0;
VehicleExtendedData<DoorsExample::VehicleDoors> DoorsExample::VehDoors;
