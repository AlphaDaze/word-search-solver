#include "mainwindow.h"
#include "wordsearch/wordsearch.h"
#include <QtWidgets>
#include <QtAlgorithms>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    createActions();
    createMenus();
    setupUi();
    setWindowIcon(QIcon(":icon.ico"));
}

void MainWindow::newFile()
{
    if (okToContinue())
    {
        wordSearch->clear();
        wordSearch->resize(wordSearch->minimumSizeHint());
        refreshWordSearch();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (okToContinue())
    {
        const QString &fileName = QFileDialog::getOpenFileName(this, tr("Open WordSearchSolver or Image File"), ".",
                            tr("All Files(*)"));
        if (!fileName.isEmpty())
                loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty())
        return saveAs();
    else
        return saveFile(curFile);
}

bool MainWindow::saveAs()
{
    const QString &fileName = QFileDialog::getSaveFileName(this, tr("Save WordSearch"), ".",
                                                    tr("WordSearchSolver files (*.wss)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About WordSearchSolver"),
                       tr("<h2>WordSearchSolver 1.1</h2>"
                          "<p>Copyright (c) 2016 ChajusSaib"
                          "<p>WordSearchSolver helps you solve WordSearchs fast!"));
}


void MainWindow::addWord()
{
    QString word = wordInput->text();
    word = word.toUpper();
    word = word.simplified();
    word.replace(" ", "");

    findWordsModel->insertRow(findWordsModel->rowCount());
    QModelIndex index = findWordsModel->index(findWordsModel->rowCount() - 1);
    findWordsModel->setData(index, word);

    wordSearch->find(word);
}

void MainWindow::setupUi()
{
    QWidget *centralWidget = new QWidget(this);

    wordInput = new QLineEdit(centralWidget);
    enterWordButton = new QPushButton(centralWidget);

    wordInput->setPlaceholderText(tr("Enter Word"));
    wordInput->setMaximumWidth(105);
    enterWordButton->setText(tr("Find Word"));
    enterWordButton->setMaximumWidth(105);

    connect(enterWordButton, SIGNAL(pressed()), this, SLOT(addWord()));

    wordSearch = new WordSearch();
    wordSearchScrollArea = new QScrollArea;
    wordSearchScrollArea->setWidget(wordSearch);
    wordSearchScrollArea->setWidgetResizable(true);

    findWordsListView = new QListView(this);
    findWordsModel = new QStringListModel(this);

    findWordsListView->setMaximumWidth(105);
    findWordsListView->setModel(findWordsModel);
    findWordsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout *centralHLayout = new QHBoxLayout(centralWidget);
    QVBoxLayout *rightLayout = new QVBoxLayout;
    QVBoxLayout *bottomRightLayout = new QVBoxLayout;

    bottomRightLayout->setSpacing(0);
    bottomRightLayout->addWidget(wordInput);
    bottomRightLayout->addWidget(enterWordButton);

    rightLayout->addWidget(findWordsListView);
    rightLayout->addLayout(bottomRightLayout);

    centralHLayout->addWidget(wordSearchScrollArea, 1);
    centralHLayout->addLayout(rightLayout);

    setCentralWidget(centralWidget);
}


const QIcon *getThemeIcon(const QString &iconThemeName)
{
    QIcon *icon = new QIcon;
    if (QIcon::hasThemeIcon(iconThemeName))
        *icon = QIcon::fromTheme(iconThemeName);
    else
        icon->addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);

    return icon;
}

void MainWindow::createActions()
{
    const QIcon *newIcon = getThemeIcon("document-new");
    const QIcon *openIcon = getThemeIcon("document-open");
    const QIcon *saveIcon = getThemeIcon("document-save");
    const QIcon *saveAsIcon = getThemeIcon("document-save-as");
    const QIcon *exitIcon = getThemeIcon("application-exit");
    const QIcon *aboutIcon = getThemeIcon("help-about");

    newAction = new QAction(tr("&New"), this);
    newAction->setIcon(*newIcon);
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip(tr("Create a new WordSearch file"));
    connect(newAction, SIGNAL(triggered(bool)), this, SLOT(newFile()));

    openAction = new QAction(tr("&Open File or Image..."), this);
    openAction->setIcon(*openIcon);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing WordSearch file or image file"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(*saveIcon);
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the WordSearch to disk"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAsAction->setIcon(*saveAsIcon);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save the WordSearch under a new name"));
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setIcon(*exitIcon);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));
    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setIcon(*aboutIcon);
    aboutAction->setStatusTip(tr("Show the application's About box"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    delete newIcon;
    delete openIcon;
    delete saveIcon;
    delete saveAsIcon;
    delete exitIcon;
    delete aboutIcon;
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);

    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::refreshWordSearch()
{
    wordSearchScrollArea->resize(wordSearch->minimumSize());
    resize(wordSearch->minimumSizeHint().width() + findWordsListView->width() + 28,
           wordSearch->minimumSizeHint().height() + 40);
    update();
}

bool MainWindow::okToContinue()
{

    if (isWindowModified())
    {
        int r = QMessageBox::warning(this, tr("WordSearchSolver"),
                                     tr("The WordSearch has been modified.\n"
                                        "Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (r == QMessageBox::Yes)
            return save();
        else if (r == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool MainWindow::loadFile(const QString &fileName)
{
    if (fileName.contains("wss"))
    {
        if (!wordSearch->readFile(fileName))
            return false;
        setCurrentFile(fileName);
    }
    else
    {
        if (!wordSearch->setImageAndRun(fileName))
            return false;
        setCurrentFile(fileName);
    }

    refreshWordSearch();
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    if (!wordSearch->writeFile(fileName + ".wss"))
        return false;

    setCurrentFile(fileName);
    return true;
}


void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    // return file's name(not pathname)
    auto strippedName = [&]() -> QString { return QFileInfo(curFile).fileName(); };
    QString shownName = (curFile.isEmpty()) ? tr("Untitled") : strippedName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("WordSearchSolver")));
}
