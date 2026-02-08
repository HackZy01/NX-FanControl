#include "main_menu.hpp"

MainMenu::MainMenu()
{
    ReadConfigFile(&this->_fanCurveTable);

    bool running = (IsRunning() != 0);
    this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", running);
}

tsl::elm::Element* MainMenu::createUI()
{
    auto frame = new tsl::elm::OverlayFrame("FAN CONTROL", "v1.0.3");
    auto list = new tsl::elm::List();

    this->_enabledBtn->setStateChangedListener([this](bool state)
    {
        if (state)
        {
            CreateB2F();
            const NcmProgramLocation programLocation{
                .program_id = SysFanControlID,
                .storageID = NcmStorageId_None,
            };
            u64 pid = 0;
            pmshellLaunchProgram(0, &programLocation, &pid);
        }
        else
        {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
        }
        return true;
    });
    list->addItem(this->_enabledBtn);

    for (int i = 0; i < 5; i++)
    {
        TemperaturePoint* p = &this->_fanCurveTable[i];
        
        std::string headerStr = std::to_string(p->temperature_c) + "\u00B0C";
        list->addItem(new tsl::elm::CategoryHeader(headerStr));

        auto slider = new tsl::elm::StepTrackBar("%", 20);
        slider->setProgress((int)(p->fanLevel_f * 100) / 5);
        
        slider->setValueChangedListener([p](u8 value){
            p->fanLevel_f = (float)(value * 5) / 100.0f;
        });

        list->addItem(slider);
    }

    auto saveBtn = new tsl::elm::ListItem("Save & Apply");
    saveBtn->setClickListener([this](uint64_t keys) {
        if (keys & HidNpadButton_A) {
            WriteConfigFile(this->_fanCurveTable);

            if(IsRunning() != 0)
            {
                pmshellTerminateProgram(SysFanControlID);
                const NcmProgramLocation programLocation
                {
                    .program_id = SysFanControlID,
                    .storageID = NcmStorageId_None,
                };
                u64 pid = 0;
                pmshellLaunchProgram(0, &programLocation, &pid);
            }
            return true;
        }
        return false;
    });
    list->addItem(saveBtn);

    frame->setContent(list);
    return frame;
}

void MainMenu::update()
{
}
