#include "main_menu.hpp"

MainMenu::MainMenu()
{
    ReadConfigFile(&this->_fanCurveTable);
    this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", (IsRunning() != 0));
}

tsl::elm::Element* MainMenu::createUI()
{
    auto frame = new tsl::elm::OverlayFrame("FAN CONTROL", "v1.0.3");
    auto list = new tsl::elm::List();

    // 1. Service Toggle
    this->_enabledBtn->setStateChangedListener([](bool state) {
        if (state) {
            CreateB2F();
            const NcmProgramLocation loc{ .program_id = SysFanControlID, .storageID = NcmStorageId_None };
            u64 pid = 0;
            pmshellLaunchProgram(0, &loc, &pid);
        } else {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
        }
        return true;
    });
    list->addItem(this->_enabledBtn);

    // 2. Sliders
    // We use simple labels and the slider below it
    const char* temps[5] = {"0°C", "40°C", "45°C", "60°C", "100°C"};

    for (int i = 0; i < 5; i++)
    {
        TemperaturePoint* p = &this->_fanCurveTable[i];

        // The requested graphical change: Label is just the Temperature
        auto label = new tsl::elm::ListItem(temps[i]);
        list->addItem(label);

        // We use 50 steps for 2% increments
        auto slider = new tsl::elm::StepTrackBar("", 50);
        slider->setProgress((int)(p->fanLevel_f * 100) / 2);
        
        slider->setValueChangedListener([p](u8 v){
            p->fanLevel_f = (float)(v * 2) / 100.0f;
        });

        list->addItem(slider);
    }

    // 3. Save & Apply
    auto saveBtn = new tsl::elm::ListItem("Save & Apply");
    saveBtn->setClickListener([this](uint64_t keys) {
        if (keys & HidNpadButton_A) {
            WriteConfigFile(this->_fanCurveTable);
            if(IsRunning() != 0) {
                pmshellTerminateProgram(SysFanControlID);
                const NcmProgramLocation loc{ .program_id = SysFanControlID, .storageID = NcmStorageId_None };
                u64 pid = 0;
                pmshellLaunchProgram(0, &loc, &pid);
            }
            return true;
        }
        return false;
    });
    list->addItem(saveBtn);

    frame->setContent(list);
    return frame;
}

void MainMenu::update() {}
