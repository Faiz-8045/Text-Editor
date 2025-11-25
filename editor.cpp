//
// Simple text editor program using the FLTK library
// Based on FLTK 1.1 documentation: https://www.fltk.org/doc-1.1/editor.html
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/fl_ask.H>

// Global variables
int changed = 0;
int loading = 0;
char filename[256] = "";
char title[256];
Fl_Text_Buffer *textbuf = 0;

// Forward declarations
class EditorWindow;
void save_cb();
void copy_cb();
void cut_cb();
void delete_cb();
void paste_cb();
void close_cb();
void quit_cb();
int check_save(void);
void load_file(char *newfile, int ipos);
void save_file(char *newfile);
void set_title(Fl_Window* w);
void new_cb(Fl_Widget*, void*);
void open_cb(Fl_Widget*, void*);
void insert_cb(Fl_Widget*, void*);
void saveas_cb(Fl_Widget*, void*);
void find_cb(Fl_Widget*, void*);
void find2_cb(Fl_Widget*, void*);
void replace_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);
void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v);
void undo_cb(Fl_Widget*, void*);

// EditorWindow class
class EditorWindow : public Fl_Double_Window {
public:
    EditorWindow(int w, int h, const char* t);
    ~EditorWindow();
    
    Fl_Window          *replace_dlg;
    Fl_Input           *replace_find;
    Fl_Input           *replace_with;
    Fl_Button          *replace_all;
    Fl_Return_Button   *replace_next;
    Fl_Button          *replace_cancel;
    
    Fl_Text_Editor     *editor;
    char               search[256];
};

EditorWindow *w = (EditorWindow *)0;

// Menu items
Fl_Menu_Item menuitems[] = {
    { "&File",              0, 0, 0, FL_SUBMENU },
        { "&New File",        0, (Fl_Callback *)new_cb },
        { "&Open File...",    FL_CTRL + 'o', (Fl_Callback *)open_cb },
        { "&Insert File...",  FL_CTRL + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
        { "&Save File",       FL_CTRL + 's', (Fl_Callback *)save_cb },
        { "Save File &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
        { "E&xit",            FL_CTRL + 'q', (Fl_Callback *)quit_cb, 0 },
        { 0 },
    { "&Edit", 0, 0, 0, FL_SUBMENU },
        { "&Undo",       FL_CTRL + 'z', (Fl_Callback *)undo_cb, 0, FL_MENU_DIVIDER },
        { "Cu&t",        FL_CTRL + 'x', (Fl_Callback *)cut_cb },
        { "&Copy",       FL_CTRL + 'c', (Fl_Callback *)copy_cb },
        { "&Paste",      FL_CTRL + 'v', (Fl_Callback *)paste_cb },
        { "&Delete",     0, (Fl_Callback *)delete_cb },
        { 0 },
    { "&Search", 0, 0, 0, FL_SUBMENU },
        { "&Find...",       FL_CTRL + 'f', (Fl_Callback *)find_cb },
        { "F&ind Again",    FL_CTRL + 'g', find2_cb },
        { "&Replace...",    FL_CTRL + 'h', replace_cb },
        { "Re&place Again", FL_CTRL + 't', replace2_cb },
        { 0 },
    { 0 }
};

// EditorWindow constructor
EditorWindow::EditorWindow(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
    replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(70, 10, 200, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);
    
    replace_with = new Fl_Input(70, 40, 200, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);
    
    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);
    
    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);
    
    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback((Fl_Callback *)replcan_cb, this);
    replace_dlg->end();
    replace_dlg->set_non_modal();
    editor = 0;
    *search = (char)0;
}

// EditorWindow destructor
EditorWindow::~EditorWindow() {
    delete replace_dlg;
}

// Callback functions
void changed_cb(int, int nInserted, int nDeleted, int, const char*, void* v) {
    if ((nInserted || nDeleted) && !loading) changed = 1;
    EditorWindow *w = (EditorWindow *)v;
    set_title(w);
    if (loading) w->editor->show_insert_position();
}

void copy_cb() {
    EditorWindow* e = w;
    Fl_Text_Editor::kf_copy(0, e->editor);
}

void cut_cb() {
    EditorWindow* e = w;
    Fl_Text_Editor::kf_cut(0, e->editor);
    changed = 1;
    set_title(w);
}

void delete_cb() {
    textbuf->remove_selection();
    changed = 1;
    set_title(w);
}

void find_cb(Fl_Widget* widget, void*) {
    EditorWindow* e = w;
    const char *val;
    
    val = fl_input("Search String:", e->search);
    if (val != NULL) {
        strcpy(e->search, val);
        find2_cb(widget, NULL);
    }
}

void find2_cb(Fl_Widget* widget, void*) {
    EditorWindow* e = w;
    if (e->search[0] == '\0') {
        find_cb(widget, NULL);
        return;
    }
    
    int pos = e->editor->insert_position();
    int found = textbuf->search_forward(pos, e->search, &pos);
    
    if (found) {
        textbuf->select(pos, pos + strlen(e->search));
        e->editor->insert_position(pos + strlen(e->search));
        e->editor->show_insert_position();
    }
    else fl_alert("No occurrences of \'%s\' found!", e->search);
}

void new_cb(Fl_Widget*, void*) {
    if (!check_save()) return;
    
    filename[0] = '\0';
    textbuf->select(0, textbuf->length());
    textbuf->remove_selection();
    changed = 0;
    textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void*) {
    if (!check_save()) return;
    
    char *newfile = fl_file_chooser("Open File?", "*", filename);
    if (newfile != NULL) load_file(newfile, -1);
}

void insert_cb(Fl_Widget*, void*) {
    char *newfile = fl_file_chooser("Insert File?", "*", filename);
    if (newfile != NULL) load_file(newfile, w->editor->insert_position());
}

void paste_cb() {
    EditorWindow* e = w;
    Fl_Text_Editor::kf_paste(0, e->editor);
    changed = 1;
    set_title(w);
}

void quit_cb() {
    if (changed && !check_save())
        return;
    
    exit(0);
}

void replace_cb(Fl_Widget*, void*) {
    w->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void*) {
    const char *find = w->replace_find->value();
    const char *replace = w->replace_with->value();
    
    if (find[0] == '\0') {
        w->replace_dlg->show();
        return;
    }
    
    w->replace_dlg->hide();
    
    int pos = w->editor->insert_position();
    int found = textbuf->search_forward(pos, find, &pos);
    
    if (found) {
        textbuf->select(pos, pos + strlen(find));
        textbuf->remove_selection();
        textbuf->insert(pos, replace);
        textbuf->select(pos, pos + strlen(replace));
        w->editor->insert_position(pos + strlen(replace));
        w->editor->show_insert_position();
        changed = 1;
        set_title(w);
    }
    else fl_alert("No occurrences of \'%s\' found!", find);
}

void replall_cb(Fl_Widget*, void*) {
    const char *find = w->replace_find->value();
    const char *replace = w->replace_with->value();
    
    find = w->replace_find->value();
    if (find[0] == '\0') {
        w->replace_dlg->show();
        return;
    }
    
    w->replace_dlg->hide();
    
    w->editor->insert_position(0);
    int times = 0;
    
    for (int found = 1; found;) {
        int pos = w->editor->insert_position();
        found = textbuf->search_forward(pos, find, &pos);
        
        if (found) {
            textbuf->select(pos, pos + strlen(find));
            textbuf->remove_selection();
            textbuf->insert(pos, replace);
            w->editor->insert_position(pos + strlen(replace));
            w->editor->show_insert_position();
            times++;
        }
    }
    
    if (times) {
        changed = 1;
        set_title(w);
    }
    
    fl_message("Replaced %d occurrences.", times);
}

void replcan_cb(Fl_Widget*, void*) {
    w->replace_dlg->hide();
}

void save_cb() {
    if (filename[0] == '\0') {
        saveas_cb(NULL, NULL);
        return;
    }
    else save_file(filename);
}

void saveas_cb(Fl_Widget*, void*) {
    char *newfile;
    
    newfile = fl_file_chooser("Save File As?", "*", filename);
    if (newfile != NULL) save_file(newfile);
}

void undo_cb(Fl_Widget*, void*) {
    Fl_Text_Editor::kf_undo(0, w->editor);
    changed = 1;
    set_title(w);
}

// Helper functions
int check_save(void) {
    if (!changed) return 1;
    
    int r = fl_choice("The current file has not been saved.\nWould you like to save it now?",
                      "Cancel", "Save", "Don't Save");
    
    if (r == 1) {
        save_cb();
        return !changed;
    }
    
    return (r == 2) ? 1 : 0;
}

void load_file(char *newfile, int ipos) {
    loading = 1;
    int insert = (ipos != -1);
    changed = insert;
    if (!insert) strcpy(filename, "");
    int r;
    if (!insert) r = textbuf->loadfile(newfile);
    else r = textbuf->insertfile(newfile, ipos);
    
    if (r)
        fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
    else
        if (!insert) strcpy(filename, newfile);
    loading = 0;
    textbuf->call_modify_callbacks();
}

void save_file(char *newfile) {
    if (textbuf->savefile(newfile))
        fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
    else
        strcpy(filename, newfile);
    changed = 0;
    textbuf->call_modify_callbacks();
}

void set_title(Fl_Window* w) {
    if (filename[0] == '\0') strcpy(title, "Untitled");
    else {
        char *slash;
        slash = strrchr(filename, '/');
#ifdef WIN32
        if (slash == NULL) slash = strrchr(filename, '\\');
#endif
        if (slash != NULL) strcpy(title, slash + 1);
        else strcpy(title, filename);
    }
    
    if (changed) strcat(title, " (modified)");
    
    w->label(title);
}

// Main function
int main(int argc, char **argv) {
    textbuf = new Fl_Text_Buffer;
    
    w = new EditorWindow(640, 480, title);
    w->callback((Fl_Callback *)quit_cb);
    
    Fl_Menu_Bar *m = new Fl_Menu_Bar(0, 0, 640, 30);
    m->copy(menuitems, w);
    
    w->editor = new Fl_Text_Editor(0, 30, 640, 450);
    w->editor->buffer(textbuf);
    w->editor->textfont(FL_COURIER);
    textbuf->add_modify_callback(changed_cb, w);
    
    w->end();
    w->resizable(w->editor);
    w->size_range(300, 200);
    w->show(1, argv);
    
    if (argc > 1)
        load_file(argv[1], -1);
    
    textbuf->call_modify_callbacks();
    
    return Fl::run();
}
