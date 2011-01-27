#include "EditorArea.h"
#include "EditorCtrl.h"
#include "LiveCaret.h"
#include "MultilineDataObject.h"


class DragDropTarget : public wxDropTarget {
public:
	DragDropTarget(EditorArea& parent);
	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
	wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def) {
		m_parent.OnDragOver(x, y);
		return def;
	}

private:
	EditorArea& m_parent;
	wxFileDataObject* m_fileObject;
	wxTextDataObject* m_textObject;
	MultilineDataObject* m_columnObject;
	wxDataObjectComposite* m_dataObject;
};

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
	: m_editor(parent),
	  m_scrollPosX(0)
{ 
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

	// Set drop target so files and text can be dragged from explorer to trigger drag commands
	DragDropTarget* dropTarget = new DragDropTarget(*this);
	SetDropTarget(dropTarget);
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
	if (cpos.x < m_scrollPosX) {
		m_scrollPosX = wxMax(cpos.x - 50, 0);
		return true;
	}

	const int sizeX = m_editor.ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_scrollPosX + sizeX) {
		const int textWidth = m_editor.GetLines().GetWidth();
		const int maxScrollPos = (textWidth < sizeX) ? 0 : textWidth + m_caretWidth - sizeX; // room for caret at end
		m_scrollPosX = (cpos.x + 50) - sizeX;
		if (m_scrollPosX > maxScrollPos) m_scrollPosX = maxScrollPos;
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
	m_scrollPosX = 0;
}

bool EditorArea::IsCaretVisible() const {
	const wxSize caretsize = m_caret->GetSize();
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();

	// Check vertically
	if (cpos.y + caretsize.y < m_editor.scrollPos) return false;
	if (cpos.y >= m_editor.scrollPos + clientsize.y) return false;

	// Check horizontally
	if (cpos.x + caretsize.x < m_scrollPosX) return false;

	const int sizeX = m_editor.ClientWidthToEditor(clientsize.x);
	if (cpos.x >= m_scrollPosX + sizeX) return false;

	return true;
}

bool EditorArea::IsCaretBeyondScreen() const {
	const wxPoint cpos = m_editor.GetLines().GetCaretPos();
	const wxSize clientsize = GetClientSize();
	const wxSize caretsize = m_caret->GetSize();
	return (cpos.y + caretsize.y >= m_editor.scrollPos + clientsize.y) || (cpos.y < m_editor.scrollPos);
}

void EditorArea::MakeSelectionVisible(const wxPoint& sel_start, const wxPoint& sel_end) {
	const wxSize clientsize = GetClientSize();

	// Vertically
	if (sel_start.y < m_editor.scrollPos) m_editor.scrollPos = sel_start.y;
	else {
		const int lineheight = m_editor.GetLines().GetLineHeight();
		if (sel_end.y + lineheight >= m_editor.scrollPos + clientsize.y) {
			// Make sure that we can see as much of the selection as possible
			m_editor.scrollPos = wxMin(sel_start.y, (sel_end.y + lineheight) - clientsize.y);
		}
	}

	const int sizeX = m_editor.ClientWidthToEditor(clientsize.x);

	// Horizontally for End
	if (sel_end.x < m_scrollPosX)
		m_scrollPosX = wxMax(sel_end.x - 50, 0);
	else if (sel_end.x >= m_scrollPosX + sizeX)
		m_scrollPosX = (sel_end.x + 50) - sizeX;

	// Horizontally for Start
	if (sel_start.x < m_scrollPosX)
		m_scrollPosX = wxMax(sel_start.x - 50, 0);
	else if (sel_start.x >= m_scrollPosX + sizeX)
		m_scrollPosX = (sel_start.x + 50) - sizeX;

	// NOTE: Will first be visible on next redraw
}

bool EditorArea::UpdateScrollbar(unsigned int x) {
	// Check if we need a horizontal scrollbar
	const int scrollThumbX = GetScrollThumb(wxHORIZONTAL);
	const unsigned int width = m_editor.GetLines().GetWidth() + GetCaretWidth();
	if ((m_editor.GetLines().GetWrapMode() == cxWRAP_NONE || m_editor.m_wrapAtMargin) && width > x) {
		m_scrollPosX = wxMin(m_scrollPosX, (int)(width-x));
		m_scrollPosX = wxMax(0, m_scrollPosX);

		SetScrollbar(wxHORIZONTAL, m_scrollPosX, x, width);
	}
	else if (scrollThumbX > 0) {
		m_scrollPosX = 0;
		SetScrollbar(wxHORIZONTAL, 0, 0, 0);
		return true; // Removal of scrollbar have sent a size event
	}

	return false; // no scrollbar was added or removed
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
void EditorArea::OnLeaveWindow(wxMouseEvent& event) {m_editor.OnLeaveWindow(event);}
void EditorArea::OnScroll(wxScrollWinEvent& event) {m_editor.OnScroll(event);}
void EditorArea::OnDragDrop(const wxArrayString& filenames) {m_editor.OnDragDrop(filenames);}
void EditorArea::OnDragDropText(const wxString& text, wxDragResult dragType) {m_editor.OnDragDropText(text, dragType);}
void EditorArea::OnDragDropColumn(const wxArrayString& text, wxDragResult dragType) {m_editor.OnDragDropColumn(text, dragType);}

void EditorArea::SetScrollPosX(int pos) {
	if (pos != m_scrollPosX) {
		m_scrollPosX = pos;
		m_editor.DrawLayout();
	}
}

void EditorArea::DoHorizontalWheelScroll(wxMouseEvent& event) {
	const wxSize size = GetClientSize();
	int pos = m_scrollPosX;
	const int rotation = event.GetWheelRotation();

	if (event.GetLinesPerAction() == (int)UINT_MAX) { // signifies to scroll a page
		wxScrollWinEvent newEvent;
		newEvent.SetOrientation(wxHORIZONTAL);
		newEvent.SetEventObject(this);
		newEvent.SetEventType(rotation>0 ? wxEVT_SCROLLWIN_PAGEUP : wxEVT_SCROLLWIN_PAGEDOWN);
        ProcessEvent(newEvent);
		return;
	}

	if (rotation == 0) return;

	const int scroll_amount= (rotation / event.GetWheelDelta()) * event.GetLinesPerAction();
	pos -= 10 * scroll_amount;

	if (rotation > 0) pos = max(pos, 0); // left
	else if (rotation < 0) pos = min(pos, m_editor.GetLines().GetWidth() - (int)m_editor.GetLines().GetDisplayWidth()); // right

	if (pos != m_scrollPosX) {
		m_scrollPosX = pos;
		m_editor.DrawLayout();
	}
}

void EditorArea::OnMouseWheel(wxMouseEvent& event) {
	// If the EditorCtrl is focused, only handle the event directly if happens within the bounds
	// of the editor area. Otherwise, let if float up to the EditorFrame.

	// The EditorFrame may float the event back down, but it will do so by
	// calling EditorCtrl::ProcessMouseWheel directly.

	const wxSize& my_size = GetSize();
	const wxPoint& where = event.GetPosition();

	if ((where.x < 0) && (where.y < 0) && 
		(where.x > my_size.GetWidth()) && (where.y > my_size.GetHeight()))
	{
		ProcessMouseWheel(event);
		return;
	}

	event.Skip(true);
}

void EditorArea::ProcessMouseWheel(wxMouseEvent& event) {
	if (event.ShiftDown()) {
		// Only handle scrollwheel if we have a scrollbar
		if (GetScrollThumb(wxHORIZONTAL))
			DoHorizontalWheelScroll(event);
	}
	else {
		m_editor.ProcessVerticalMouseWheel(event);
	}
}

void EditorArea::OnDragOver(wxCoord x, wxCoord y) {
	const unsigned int lineHeight = m_editor.GetLines().GetLineHeight();
	const int scrollBorder = lineHeight / 2;
	const wxSize size = GetClientSize();

	// Check if we should scroll vertical
	if (y < scrollBorder) {
		m_editor.scrollPos = m_editor.scrollPos - (m_editor.scrollPos % lineHeight) - lineHeight;
		if (m_editor.scrollPos < 0) m_editor.scrollPos = 0;
	}
	else if (y > size.y - scrollBorder) {
		m_editor.scrollPos = m_editor.scrollPos - (m_editor.scrollPos % lineHeight) + lineHeight;
		if (m_editor.scrollPos > m_editor.GetLines().GetHeight() - size.y)
			m_editor.scrollPos = m_editor.GetLines().GetHeight() - size.y;
	}

	// Check if we should scroll horizontal
	const int editorX = m_editor.ClientWidthToEditor(x);
	const int editorWidth = m_editor.ClientWidthToEditor(size.x);
	if (editorX < scrollBorder) {
		m_scrollPosX -= 50;
		if (m_scrollPosX < 0) m_scrollPosX = 0;
	}
	else if (editorX > editorWidth - 50) {
		m_scrollPosX += 50;

		if (m_scrollPosX + editorWidth > m_editor.GetLines().GetWidth()) {
			m_scrollPosX = m_editor.GetLines().GetWidth() - editorWidth;
			m_scrollPosX = wxMax(m_scrollPosX, 0);
		}
	}

	// Move caret to insertion point
	const wxPoint mpos = m_editor.ClientPosToEditor(x, y);
	m_editor.GetLines().ClickOnLine(mpos.x, mpos.y);

	m_editor.DrawLayout();
}


// -- Editor DragDropTarget -----------------------------------------------------------------

DragDropTarget::DragDropTarget(EditorArea& parent) : m_parent(parent) {
	m_fileObject = new wxFileDataObject;
	m_textObject = new wxTextDataObject;
	m_columnObject = new MultilineDataObject;
	m_dataObject = new wxDataObjectComposite;

	// Specify the type of data we will accept
	m_dataObject->Add(m_columnObject, true /*preferred*/); // WORKAROUND: has to be first as wxDropTarget ignores preferred
	m_dataObject->Add(m_fileObject);
	m_dataObject->Add(m_textObject);
	SetDataObject(m_dataObject);
}

wxDragResult DragDropTarget::OnData(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), wxDragResult def) {
	// Copy the data from the drag source to our data object
	GetData();

	const wxDataFormat df = m_dataObject->GetReceivedFormat();
	if (df == wxDF_TEXT || df == wxDF_UNICODETEXT) {
		m_parent.OnDragDropText(m_textObject->GetText(), def);
	}
    else if (df == wxDF_FILENAME) {
		m_parent.OnDragDrop(m_fileObject->GetFilenames());
	}
	else if (df == wxDataFormat(MultilineDataObject::FormatId)) {
		wxArrayString text;
		m_columnObject->GetText(text);
		m_parent.OnDragDropColumn(text, def);
	}

	return def;
}
