#include "main_menu.hpp"

MainMenu::MainMenu()
{
    ReadConfigFile(&this->_fanCurveTable);

    if (IsRunning() != 0)
    {
        this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", true);
    }
    else
    {
        this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", false);
    }
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
            return true;
        }
        else
        {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
            return true;
        }
        return false;
    });
    list->addItem(this->_enabledBtn);

    for (int i = 0; i < 5; i++)
    {
        TemperaturePoint* p = &this->_fanCurveTable[i];
        
        std::string headerStr = std::to_string(p->temperature_c) + "\u00B0C";
        list->addItem(new tsl::elm::CategoryHeader(headerStr));

        auto slider = new tsl::elm::TrackBarListItem("Fan Speed");
        slider->setRange(0, 100);
        slider->setStep(5);
        slider->setProgress((int)(p->fanLevel_f * 100));
        
        slider->setValueChangedListener([p](u8 value){
            p->fanLevel_f = (float)value / 100.0f;
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
	    if (keys & HidNpadButton_A) 
        {
				tsl::changeTo<SelectMenu>(4, this->_fanCurveTable, &this->_tableIsChanged);
				return true;
	    }
	    return false; 
    });
    list->addItem(this->_p4Label);

    frame->setContent(list);

    return frame;
}

void MainMenu::update()
{
    if(this->_tableIsChanged)
    {
        this->_p0Label->setText("P0: " + std::to_string(this->_fanCurveTable->temperature_c) + "C | " + std::to_string((int)(this->_fanCurveTable->fanLevel_f * 100)) + "%");
        this->_p1Label->setText("P1: " + std::to_string((this->_fanCurveTable + 1)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 1)->fanLevel_f * 100)) + "%");
        this->_p2Label->setText("P2: " + std::to_string((this->_fanCurveTable + 2)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 2)->fanLevel_f * 100)) + "%");
        this->_p3Label->setText("P3: " + std::to_string((this->_fanCurveTable + 3)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 3)->fanLevel_f * 100)) + "%");
        this->_p4Label->setText("P4: " + std::to_string((this->_fanCurveTable + 4)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 4)->fanLevel_f * 100)) + "%");
        
        this->_tableIsChanged = false;
    }
}
