#include "EditorArea.h"
#include "EditorCtrl.h"

BEGIN_EVENT_TABLE(EditorArea, wxControl)
	EVT_PAINT(EditorArea::OnPaint)
	EVT_ERASE_BACKGROUND(EditorArea::OnEraseBackground)
	EVT_CHAR(EditorArea::OnChar)
	EVT_KEY_DOWN(EditorArea::OnKeyDown)
	EVT_KEY_UP(EditorArea::OnKeyUp)
	EVT_LEFT_DOWN(EditorArea::OnMouseLeftDown)
	EVT_RIGHT_DOWN(EditorArea::OnMouseRightDown)
	EVT_LEFT_UP(EditorArea::OnMouseLeftUp)
	EVT_LEFT_DCLICK(EditorArea::OnMouseDClick)
	EVT_MOTION(EditorArea::OnMouseMotion)
	EVT_MOUSE_CAPTURE_LOST(EditorArea::OnMouseCaptureLost)
	EVT_ENTER_WINDOW(EditorArea::OnMouseMotion)
	EVT_LEAVE_WINDOW(EditorArea::OnLeaveWindow)
	EVT_MOUSEWHEEL(EditorArea::OnMouseWheel)
	EVT_SCROLLWIN(EditorArea::OnScroll)
END_EVENT_TABLE()


EditorArea::EditorArea(class EditorCtrl& parent, wxWindowID id) :
	m_editor(parent) {
	Create(&parent, id, wxPoint(0,0), parent.GetSize(), wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
}

void EditorArea::OnEraseBackground(wxEraseEvent& WXUNUSED(event)) {}

void EditorArea::OnPaint(wxPaintEvent& WXUNUSED(event)) {
	wxPaintDC dc(this);
	m_editor.DrawLayout(dc);
}

bool EditorArea::OnPreKeyDown(wxKeyEvent& event) {return m_editor.OnPreKeyDown(event);}
bool EditorArea::OnPreKeyUp(wxKeyEvent& event) {return m_editor.OnPreKeyUp(event);}
void EditorArea::OnChar(wxKeyEvent& event) {m_editor.OnChar(event);}
void EditorArea::OnKeyDown(wxKeyEvent& event) {m_editor.OnKeyDown(event);}
void EditorArea::OnKeyUp(wxKeyEvent& event) {m_editor.OnKeyUp(event);}
void EditorArea::OnMouseLeftDown(wxMouseEvent& event) {m_editor.OnMouseLeftDown(event);}
void EditorArea::OnMouseRightDown(wxMouseEvent& event) {m_editor.OnMouseRightDown(event);}
void EditorArea::OnMouseLeftUp(wxMouseEvent& event) {m_editor.OnMouseLeftUp(event);}
void EditorArea::OnMouseCaptureLost(wxMouseCaptureLostEvent& event) {m_editor.OnMouseCaptureLost(event);}
void EditorArea::OnMouseDClick(wxMouseEvent& event) {m_editor.OnMouseDClick(event);}
void EditorArea::OnMouseMotion(wxMouseEvent& event) {m_editor.OnMouseMotion(event);}
void EditorArea::OnMouseWheel(wxMouseEvent& event) {m_editor.OnMouseWheel(event);}
void EditorArea::OnLeaveWindow(wxMouseEvent& event) {m_editor.OnLeaveWindow(event);}
void EditorArea::OnScroll(wxScrollWinEvent& event) {m_editor.OnScroll(event);}
