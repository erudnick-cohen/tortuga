/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Joseph Lisee
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:   tools/vision_viewer/include/PropertyControl.h
 */

#ifndef RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009
#define RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009

// Library Includes
#include <wx/panel.h>

// Project Includes
#include "core/include/Forward.h"
#include "Forward.h"

// Forward Declarations
class wxCommandEvent;
class wxTextCtrl;
class wxStaticText;
class wxCheckBox;

namespace ram {
namespace tools {
namespace visionvwr {
    
class PropertyControl : public wxPanel
{

public:
    PropertyControl(core::PropertyPtr property, Model* model,
                    wxWindow *parent, wxWindowID id = wxID_ANY,
		    const wxPoint &pos = wxDefaultPosition,
		    const wxSize &size = wxDefaultSize);
    ~PropertyControl();

    /** Returns the property to its default value */
    void setToDefault();
    
private:
    /** Called every time the text in the box changes */
    void onTextUpdated(wxCommandEvent& event);

    /** Called when the user presses enter in the text box */
    void onEnter(wxCommandEvent& event);

    /** Handles the check box being checked on and off */
    void onCheck(wxCommandEvent& event);

    /** Sets the property based on the given text */
    void setPropertyValue(wxString value);

    /** The property we are editing */
    core::PropertyPtr m_prop;

    /** Text box the users edits  */
    wxTextCtrl* m_text;

    /** Used for boolean properties */
    wxCheckBox* m_checkBox;

    /** Label for the detector */
    wxStaticText* m_label;

    /** Object running the detector */
    Model* m_model;
        
    /** The original value of the property */
    wxString m_defaultValue;

    DECLARE_EVENT_TABLE()
};

} // namespace visionvwr
} // namespace tools
} // namespace ram

#endif // RAM_TOOLS_VISION_TOOL_PROPERTYCONTROL_H_04_23_2009