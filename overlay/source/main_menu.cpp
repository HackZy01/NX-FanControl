#include "main_menu.hpp"
#include "select_menu.hpp"

MainMenu::MainMenu() {
    ReadConfigFile(&this->_fanCurveTable);

    this->_p0Label = new tsl::elm::ListItem("");
    this->_p1Label = new tsl::elm::ListItem("");
    this->_p2Label = new tsl::elm::ListItem("");
    this->_p3Label = new tsl::elm::ListItem("");
    this->_p4Label = new tsl::elm::ListItem("");
    
    this->update(); 
}

tsl::elm::Element* MainMenu::createUI() {
    auto frame = new tsl::elm::OverlayFrame("FAN CONTROL", "v1.0.3");
    auto list = new tsl::elm::List();

    // 1. Service Toggle
    bool running = (IsRunning() != 0);
    this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", running);
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

    // 2. MASTER SLIDER
    list->addItem(new tsl::elm::CategoryHeader("Master Control"));
    auto masterSlider = new tsl::elm::StepTrackBar("All Points", 50); 
    masterSlider->setValueChangedListener([this](u8 v) {
        float newLevel = (float)(v * 2) / 100.0f;
        for (int i = 0; i < 5; i++) {
            this->_fanCurveTable[i].fanLevel_f = newLevel;
        }
        this->_tableIsChanged = true;
    });
    list->addItem(masterSlider);

    // 3. RESTORED OLD MENU (Individual Points)
    list->addItem(new tsl::elm::CategoryHeader("Individual Points"));
    
    auto labels = { _p0Label, _p1Label, _p2Label, _p3Label, _p4Label };
    int i = 0;
    for (auto label : labels) {
        label->setClickListener([this, i](uint64_t keys) {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SelectMenu>(i, this->_fanCurveTable, &this->_tableIsChanged);
                return true;
            }
            return false;
        });
        list->addItem(label);
        i++;
    }

    frame->setContent(list);
    return frame;
}

void MainMenu::update() {
    this->_p0Label->setText("0C   -> " + std::to_string((int)(this->_fanCurveTable[0].fanLevel_f * 100)) + "%");
    this->_p1Label->setText("40C  -> " + std::to_string((int)(this->_fanCurveTable[1].fanLevel_f * 100)) + "%");
    this->_p2Label->setText("45C  -> " + std::to_string((int)(this->_fanCurveTable[2].fanLevel_f * 100)) + "%");
    this->_p3Label->setText("60C  -> " + std::to_string((int)(this->_fanCurveTable[3].fanLevel_f * 100)) + "%");
    this->_p4Label->setText("100C -> " + std::to_string((int)(this->_fanCurveTable[4].fanLevel_f * 100)) + "%");
}
