#include "EditorArea.h"
#include "EditorCtrl.h"
#include "LiveCaret.h"

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

// initialize statics
const unsigned int EditorArea::m_caretWidth = 2;

EditorArea::EditorArea(class EditorCtrl& parent, wxWindowID id)
	: m_editor(parent) {
	Create(&parent, id, wxPoint(0,0), parent.GetSize(), wxNO_BORDER|wxWANTS_CHARS|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);
	Init();
}

void EditorArea::Init() {
	// Create a caret to indicate edit position
	m_caretHeight = m_editor.GetLines().GetLineHeight();
	m_caret = new LiveCaret(this, m_caretWidth, m_caretHeight); // will be deleted by window on destroy
	m_caret->Move(0, 0);
	SetCaret(m_caret);
	m_caret->Show();

	SetCursor(wxCursor(wxCURSOR_IBEAM));
}

void EditorArea::HideCaret() {
	if (m_caret->IsVisible()) m_caret->Hide();
}

void EditorArea::SetCaretPos(const wxPoint& cpos) {
	m_caret->Move(cpos);
	m_caret->Show();
}

void EditorArea::KeepCaretAlive(bool keepAlive) {
	m_caret->KeepAlive(keepAlive);
}

void EditorArea::SetCaretSize(unsigned int sizeY) {
	m_caretHeight = sizeY;
	m_caret->SetSize(m_caretWidth, m_caretHeight);
}

bool EditorArea::MakeCaretVisible() {
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check if the caret have moved outside visible display vertically
	if (cpos.y < m_editor.scrollPos) {
		m_editor.scrollPos = cpos.y;
		return true;
	}

	const int lineheight = m_editor.GetLines().GetLineHeight();
	if (cpos.y + lineheight > m_editor.scrollPos + clientsize.y) {
		m_editor.scrollPos = (cpos.y + lineheight) - clientsize.y;
		return true;
	}

	// Check if the caret have moved outside visible display horizontally
	if (cpos.x < m_editor.m_scrollPosX) {
		m_editor.m_scrollPosX = wxMax(cpos.x - 50, 0);
		return true;
	}

	const int sizeX = m_editor.ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_editor.m_scrollPosX + sizeX) {
		const int textWidth = m_editor.GetLines().GetWidth();
		const int maxScrollPos = (textWidth < sizeX) ? 0 : textWidth + m_caretWidth - sizeX; // room for caret at end
		m_editor.m_scrollPosX = (cpos.x + 50) - sizeX;
		if (m_editor.m_scrollPosX > maxScrollPos) m_editor.m_scrollPosX = maxScrollPos;
		return true;
	}

	return false; // no movement
	// NOTE: Will first be visible on next redraw
}

void EditorArea::MakeCaretVisibleCenter() {
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();

	m_editor.scrollPos = cpos.y - (clientsize.y / 2);
	if (m_editor.scrollPos < 0) m_editor.scrollPos = 0;

	// Go to start of line
	m_editor.m_scrollPosX = 0;
}

bool EditorArea::IsCaretVisible() const {
	const wxSize caretsize = m_caret->GetSize();
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check vertically
	if (cpos.y + caretsize.y < m_editor.scrollPos) return false;
	if (cpos.y >= m_editor.scrollPos + clientsize.y) return false;

	// Check horizontally
	if (cpos.x + caretsize.x < m_editor.m_scrollPosX) return false;

	const int sizeX = m_editor.ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_editor.m_scrollPosX + sizeX) return false;

	return true;
}

bool EditorArea::IsCaretBeyondScreen() const {
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();
	const wxSize caretsize = m_caret->GetSize();
	return (cpos.y + caretsize.y >= m_editor.scrollPos + clientsize.y) || (cpos.y < m_editor.scrollPos);
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
