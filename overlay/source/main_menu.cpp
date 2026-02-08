#include "main_menu.hpp"

MainMenu::MainMenu() {
    ReadConfigFile(&this->_fanCurveTable);
    bool running = (IsRunning() != 0);
    this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", running);
}

tsl::elm::Element* MainMenu::createUI() {
    auto frame = new tsl::elm::OverlayFrame("FAN CONTROL", "v1.0.3");
    auto list = new tsl::elm::List();

    this->_enabledBtn->setStateChangedListener([this](bool state) {
        if (state) {
            CreateB2F();
            const NcmProgramLocation programLocation{
                .program_id = SysFanControlID,
                .storageID = NcmStorageId_None,
            };
            u64 pid = 0;
            pmshellLaunchProgram(0, &programLocation, &pid);
        } else {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
        }
        return true;
    });
    list->addItem(this->_enabledBtn);

    const char* labels[5] = {"0°C", "40°C", "45°C", "60°C", "100°C"};

    for (int i = 0; i < 5; i++) {
        TemperaturePoint* p = &this->_fanCurveTable[i];
        
        list->addItem(new tsl::elm::CategoryHeader(labels[i]));

        // 50 steps = 2% increments
        auto slider = new tsl::elm::StepTrackBar("%", 50);
        slider->setProgress((int)(p->fanLevel_f * 100) / 2);
        
        slider->setValueChangedListener([p](u8 value){
            p->fanLevel_f = (float)(value * 2) / 100.0f;
        });

        list->addItem(slider);
    }

    auto saveBtn = new tsl::elm::ListItem("Save & Apply");
    saveBtn->setClickListener([this](uint64_t keys) {
        if (keys & HidNpadButton_A) {
            WriteConfigFile(this->_fanCurveTable);
            if(IsRunning() != 0) {
                pmshellTerminateProgram(SysFanControlID);
                const NcmProgramLocation programLocation {
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

void MainMenu::update() {}
