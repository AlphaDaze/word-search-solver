#ifndef WordSearch_H
#define WordSearch_H

#include <QWidget>
#include <QString>
#include <QSet>
#include <QRect>

class QImage;

class WordSearch : public QWidget
{
    Q_OBJECT

public:
    typedef QString::size_type size_type;

    WordSearch(QWidget *parent = 0);

    bool setImageAndRun(QString imageFile);
    void clear();

    bool writeFile(const QString &fileName);
    bool readFile(const QString &fileName);

    void find(const QString &word);

    QSize minimumSizeHint() const override;

signals:
    void foundWord(QSet<QString::size_type> positions);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    enum { MagicNumber = 0x7F51C883 };

    bool getText(QString &imageFile);
    void findWidthHeightSize();

    QString wordSearchContents;
    size_type lineSize = 0;
    size_type rowSize = 0;
    QSet<QString::size_type> positions;

    QRect idealSize;
};

#endif // WordSearch_H
