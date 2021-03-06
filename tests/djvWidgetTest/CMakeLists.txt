set(header
    AbstractWidgetTest.h
    AboutDialogTest.h
    ChoiceDialogTest.h
    ColorWidgetTest.h
    ColorDialogTest.h
    FileBrowserTest.h
    FileEditTest.h
    IconLibraryTest.h
    InfoDialogTest.h
    InputDialogTest.h
    IntSliderTest.h
    IconLibraryTest.h
    MessagesDialogTest.h
    MultiChoiceDialogTest.h
    NoticeDialogTest.h
    NumWidgetTest.h
    PixelWidgetTest.h
    PrefsDialogTest.h
    ProgressDialogTest.h
    QuestionDialogTest.h
    SearchBoxTest.h
    ShortcutsWidgetTest.h
    ToolButtonTest.h
    VectorWidgetTest.h
    ViewWidgetTest.h
    WidgetTestApplication.h
    WidgetTestModel.h
    WidgetTestManager.h
    WidgetTestWindow.h)
set(mocHeader
    AbstractWidgetTest.h
    AboutDialogTest.h
    ChoiceDialogTest.h
    ColorDialogTest.h
    ColorWidgetTest.h
    FileBrowserTest.h
    FileEditTest.h
    IconLibraryTest.h
    InfoDialogTest.h
    InputDialogTest.h
    IntSliderTest.h
    MessagesDialogTest.h
    MultiChoiceDialogTest.h
    NoticeDialogTest.h
    NumWidgetTest.h
    PixelWidgetTest.h
    PrefsDialogTest.h
    ProgressDialogTest.h
    QuestionDialogTest.h
    SearchBoxTest.h
    ShortcutsWidgetTest.h
    ToolButtonTest.h
    VectorWidgetTest.h
    ViewWidgetTest.h
    WidgetTestApplication.h
    WidgetTestManager.h
    WidgetTestWindow.h)
set(source
    AbstractWidgetTest.cpp
    AboutDialogTest.cpp
    ChoiceDialogTest.cpp
    ColorWidgetTest.cpp
    ColorDialogTest.cpp
    FileBrowserTest.cpp
    FileEditTest.cpp
    IconLibraryTest.cpp
    InfoDialogTest.cpp
    InputDialogTest.cpp
    IntSliderTest.cpp
    MessagesDialogTest.cpp
    MultiChoiceDialogTest.cpp
    NoticeDialogTest.cpp
    NumWidgetTest.cpp
    PixelWidgetTest.cpp
    PrefsDialogTest.cpp
    ProgressDialogTest.cpp
    QuestionDialogTest.cpp
    SearchBoxTest.cpp
    ShortcutsWidgetTest.cpp
    ToolButtonTest.cpp
    VectorWidgetTest.cpp
    ViewWidgetTest.cpp
    WidgetTestApplication.cpp
    WidgetTestModel.cpp
    WidgetTestManager.cpp
    WidgetTestWindow.cpp)

QT5_WRAP_CPP(mocSource ${mocHeader})

include_directories(
    ${CMAKE_SOURCE_DIR}/tests/djvWidgetTest
    ${OPENGL_INCLUDE_DIRS})
add_executable(djvWidgetTest ${header} ${source} ${mocSource})
target_link_libraries(djvWidgetTest djvViewLib)
set_target_properties(djvWidgetTest PROPERTIES FOLDER tests CXX_STANDARD 11)

