#include "main_menu.hpp"

MainMenu::MainMenu() {
    ReadConfigFile(&this->_fanCurveTable);
}

tsl::elm::Element* MainMenu::createUI() {
    auto frame = new tsl::elm::OverlayFrame("FAN CONTROL", "v1.0.3");
    auto list = new tsl::elm::List();

    // 1. Enabled Toggle
    bool running = (IsRunning() != 0);
    auto enabledBtn = new tsl::elm::ToggleListItem("Enabled", running);
    enabledBtn->setStateChangedListener([](bool state) {
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
    list->addItem(enabledBtn);

    // 2. Hardcoded Points (P0 - P4) to prevent any loop overhead
    // P0 - 0C
    list->addItem(new tsl::elm::CategoryHeader("0°C"));
    auto s0 = new tsl::elm::StepTrackBar("%", 50);
    s0->setProgress((int)(this->_fanCurveTable[0].fanLevel_f * 100) / 2);
    s0->setValueChangedListener([this](u8 v){ this->_fanCurveTable[0].fanLevel_f = (float)(v * 2) / 100.0f; });
    list->addItem(s0);

    // P1 - 40C
    list->addItem(new tsl::elm::CategoryHeader("40°C"));
    auto s1 = new tsl::elm::StepTrackBar("%", 50);
    s1->setProgress((int)(this->_fanCurveTable[1].fanLevel_f * 100) / 2);
    s1->setValueChangedListener([this](u8 v){ this->_fanCurveTable[1].fanLevel_f = (float)(v * 2) / 100.0f; });
    list->addItem(s1);

    // P2 - 45C
    list->addItem(new tsl::elm::CategoryHeader("45°C"));
    auto s2 = new tsl::elm::StepTrackBar("%", 50);
    s2->setProgress((int)(this->_fanCurveTable[2].fanLevel_f * 100) / 2);
    s2->setValueChangedListener([this](u8 v){ this->_fanCurveTable[2].fanLevel_f = (float)(v * 2) / 100.0f; });
    list->addItem(s2);

    // P3 - 60C
    list->addItem(new tsl::elm::CategoryHeader("60°C"));
    auto s3 = new tsl::elm::StepTrackBar("%", 50);
    s3->setProgress((int)(this->_fanCurveTable[3].fanLevel_f * 100) / 2);
    s3->setValueChangedListener([this](u8 v){ this->_fanCurveTable[3].fanLevel_f = (float)(v * 2) / 100.0f; });
    list->addItem(s3);

    // P4 - 100C
    list->addItem(new tsl::elm::CategoryHeader("100°C"));
    auto s4 = new tsl::elm::StepTrackBar("%", 50);
    s4->setProgress((int)(this->_fanCurveTable[4].fanLevel_f * 100) / 2);
    s4->setValueChangedListener([this](u8 v){ this->_fanCurveTable[4].fanLevel_f = (float)(v * 2) / 100.0f; });
    list->addItem(s4);

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
