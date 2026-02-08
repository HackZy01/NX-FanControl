#pragma once
#include <tesla.hpp>
#include <fancontrol.h>
#include "utils.hpp"

class MainMenu : public tsl::Gui 
{
private:
    TemperaturePoint* _fanCurveTable;
    tsl::elm::ToggleListItem* _enabledBtn;

public:
    MainMenu();

    virtual tsl::elm::Element* createUI() override;
    virtual void update() override;
};
