#ifndef __EDITORAREA_H__
#define __EDITORAREA_H__

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "key_hook.h"

class LiveCaret;
class EditorCtrl;
class EditorArea : public KeyHookable<wxControl> {
public:
	EditorArea(class EditorCtrl& parent, wxWindowID id);

	unsigned int GetHeight() const {return GetClientSize().y;}

	void KeepCaretAlive(bool keepalive);
	bool IsCaretVisible() const;
	bool IsCaretBeyondScreen() const;
	void SetCaretPos(const wxPoint& cpos);
	void SetCaretSize(unsigned int sizeY);
	void HideCaret();
	bool MakeCaretVisible();
	void MakeCaretVisibleCenter();
	unsigned int GetCaretWidth() const {return m_caretWidth;}
	unsigned int GetCaretHeight() const {return m_caretHeight;}

protected:
	virtual bool OnPreKeyDown(wxKeyEvent& event);
	virtual bool OnPreKeyUp(wxKeyEvent& event);

private:
	void Init();

	// Event handlers
	void OnPaint(wxPaintEvent& event);
	void OnEraseBackground(wxEraseEvent& event);
	void OnChar(wxKeyEvent& event);
	void OnKeyDown(wxKeyEvent& event);
	void OnKeyUp(wxKeyEvent& event);
	void OnMouseLeftDown(wxMouseEvent& event);
	void OnMouseRightDown(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnMouseCaptureLost(wxMouseCaptureLostEvent& event);
	void OnMouseDClick(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	void OnMouseWheel(wxMouseEvent& event);
	void OnLeaveWindow(wxMouseEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	DECLARE_EVENT_TABLE();

	EditorCtrl& m_editor;

	LiveCaret* m_caret;
	static const unsigned int m_caretWidth;
	unsigned int m_caretHeight;
};

#endif // __EDITORAREA_H__