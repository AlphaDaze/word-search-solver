#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QLineEdit;
class QStringListModel;
class QListView;
class WordSearch;
class QScrollArea;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();

    void addWord();

private:
    void setupUi();
    void createActions();
    void createMenus();

    void refreshWordSearch();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    QString curFile;
    WordSearch *wordSearch;
    QScrollArea *wordSearchScrollArea;

    QListView *findWordsListView;
    QStringListModel *findWordsModel;
    QLineEdit *wordInput;
    QPushButton *enterWordButton;

    QMenu *fileMenu;
    QMenu *helpMenu;

    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
};

#endif
