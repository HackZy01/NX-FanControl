#include "main_menu.hpp"
#include "select_menu.hpp"

MainMenu::MainMenu()
{
    ReadConfigFile(&this->_fanCurveTable);

    // Initialize the slider: 21 steps (0% to 100% in 5% increments)
    this->_masterSlider = new tsl::elm::StepTrackBar("Master Fan Level", 20);

    this->_p0Label = new tsl::elm::ListItem("P0: " + std::to_string(this->_fanCurveTable->temperature_c) + "C | " + std::to_string((int)(this->_fanCurveTable->fanLevel_f * 100)) + "%");
    this->_p1Label = new tsl::elm::ListItem("P1: " + std::to_string((this->_fanCurveTable + 1)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 1)->fanLevel_f * 100)) + "%");
    this->_p2Label = new tsl::elm::ListItem("P2: " + std::to_string((this->_fanCurveTable + 2)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 2)->fanLevel_f * 100)) + "%");
    this->_p3Label = new tsl::elm::ListItem("P3: " + std::to_string((this->_fanCurveTable + 3)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 3)->fanLevel_f * 100)) + "%");
    this->_p4Label = new tsl::elm::ListItem("P4: " + std::to_string((this->_fanCurveTable + 4)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 4)->fanLevel_f * 100)) + "%");

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
    this->_tableIsChanged = false;

    auto frame = new tsl::elm::OverlayFrame("NX-FanControl", "v1.0.3");
    auto list = new tsl::elm::List();

    // Enabled Toggle Logic
    this->_enabledBtn->setStateChangedListener([this](bool state)
    {
        if (state) {
            CreateB2F();
            const NcmProgramLocation programLocation{ .program_id = SysFanControlID, .storageID = NcmStorageId_None };
            u64 pid = 0;
            pmshellLaunchProgram(0, &programLocation, &pid);
        } else {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
        }
        return true;
    });
    list->addItem(this->_enabledBtn);

    // --- Master Slider Configuration ---
    this->_masterSlider->setProgress((int)(this->_fanCurveTable->fanLevel_f * 100) / 5);
    this->_masterSlider->setValueChangedListener([this](u8 value) {
        float newLevel = (float)(value * 5) / 100.0f;
        for(int i = 0; i < 5; i++) {
            (this->_fanCurveTable + i)->fanLevel_f = newLevel;
        }
        this->_tableIsChanged = true;
    });
    list->addItem(this->_masterSlider);
    // ------------------------------------

    list->addItem(new tsl::elm::CategoryHeader("Fan Curve", true));

    // Point Listeners
    auto addPoint = [this, list](tsl::elm::ListItem* label, int index) {
        label->setClickListener([this, index](uint64_t keys) {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<SelectMenu>(index, this->_fanCurveTable, &this->_tableIsChanged);
                return true;
            }
            return false;
        });
        list->addItem(label);
    };

    addPoint(this->_p0Label, 0);
    addPoint(this->_p1Label, 1);
    addPoint(this->_p2Label, 2);
    addPoint(this->_p3Label, 3);
    addPoint(this->_p4Label, 4);

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
        
        WriteConfigFile(this->_fanCurveTable); // Save changes to disk
        this->_tableIsChanged = false;
    }
}
		}
		return false; 
    });
    list->addItem(this->_p2Label);

    this->_p3Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
		    tsl::changeTo<SelectMenu>(3, this->_fanCurveTable, &this->_tableIsChanged);
			return true;
		}
		return false; 
    });
    list->addItem(this->_p3Label);

    this->_p4Label->setClickListener([this](uint64_t keys)
    {
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
