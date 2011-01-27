/*******************************************************************************
 *
 * Copyright (C) 2009, Alexander Stigsen, e-texteditor.com
 *
 * This software is licensed under the Open Company License as described
 * in the file license.txt, which you should have received as part of this
 * distribution. The terms are also available at http://opencompany.org/license.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ******************************************************************************/

#ifndef __COMPLETIONPOPUP_H__
#define __COMPLETIONPOPUP_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
   #include <wx/wx.h>
#endif

class EditorCtrl;

class CompletionPopup: public wxDialog {
public:
	CompletionPopup(wxControl* parent, EditorCtrl& editor, const wxPoint& pos, const wxPoint& topPos, const wxString& target, const wxArrayString& completions);
};

#endif // __COMPLETIONPOPUP_H__
