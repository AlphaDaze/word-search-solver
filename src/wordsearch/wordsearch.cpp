#include <QtWidgets>
#include "wordsearch/wordsearch.h"
#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>

WordSearch::WordSearch(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


bool WordSearch::setImageAndRun(QString imageFile)
{
    clear();
    if (getText(imageFile))
    {
        findWidthHeightSize();
        resize(minimumSizeHint());
        return true;
    }
    return false;
}

void WordSearch::clear()
{
    wordSearchContents = "";
    lineSize = 0;
    rowSize = 0;
    positions.clear();
}


bool WordSearch::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("WordSearchSolver"),
                             tr("Cannot write file %1:\n%2").arg(file.fileName()).arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_4);

    out << quint32(MagicNumber);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << wordSearchContents << lineSize << rowSize << positions;
    QApplication::restoreOverrideCursor();
    return true;
}

bool WordSearch::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("WordSearchSolver"),
                             tr("Cannot read file %1:\n%2").arg(file.fileName()).arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_4);

    quint32 magic;
    in >> magic;
    if (magic != MagicNumber)
    {
        QMessageBox::warning(this, tr("WordSearchSolver"),
                                 tr("The file is not a Word Search Solver file."));
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    in >> wordSearchContents >> lineSize >> rowSize >> positions;
    QApplication::restoreOverrideCursor();
    resize(minimumSizeHint());
    return true;
}


void WordSearch::find(const QString &word)
{
    if (word.size() > 1)
    {
        QSet<QString::size_type> curPositions;

        for (size_type ind = 0; ind != wordSearchContents.size(); ++ind)
        {
            if (word[0] == wordSearchContents[ind])     // first letter found
            {
                // counters
                size_type leftToRight = 1;
                size_type rightToLeft = 1;
                size_type topToBottom = 1;
                size_type bottomToTop = 1;

                size_type topToBottomRight = 1;  // diagonal going down to the right
                size_type bottomToTopRight = 1;
                size_type topToBottomLeft = 1;
                size_type bottomToTopLeft = 1;  // diagonal going to the top left

                auto incrCounter = [](size_type &cnt, bool &stop) { ++cnt; stop = false; };

                size_type i = 1;
                // checks if next letter in the word search matces next letter of the word
                auto rightLetterMatches = [&]() { return ind + i < wordSearchContents.size() &&  word[i] == wordSearchContents[ind + i]; };
                auto leftLetterMatches = [&]() { return ind - i >= 0 && word[i] == wordSearchContents[ind - i]; };
                auto letterBelowMatches = [&]() { return ind + i * lineSize < wordSearchContents.size() && word[i] == wordSearchContents[ind + i * lineSize + i]; };
                auto letterAboveMatches = [&]() { return ind - i * lineSize - i >= 0 && word[i] == wordSearchContents[ind - i * lineSize - i]; };

                auto bottomRightLetterMatches =
                                    [&]() { return ind + i * lineSize + i * 2 < wordSearchContents.size() && word[i] == wordSearchContents[ind + i * lineSize + i * 2]; };
                auto topRightLetterMatches =
                                    [&]() { return ind - i * lineSize >= 0 && word[i] == wordSearchContents[ind - i * lineSize]; };
                auto bottomLeftLetterMatches =
                                    [&]() { return ind + i * lineSize - i < wordSearchContents.size() && word[i] == wordSearchContents[ind + i * lineSize]; };
                auto topLeftLetterMatches =
                                    [&]() { return ind - i * lineSize - i * 2 >= 0 && word[i] == wordSearchContents[ind - i * lineSize - i * 2]; };

                for (bool stop = false; !stop && i != word.size(); ++i)
                {
                    stop = true;

                    if (rightLetterMatches())
                        incrCounter(leftToRight, stop);

                    if (leftLetterMatches())
                        incrCounter(rightToLeft, stop);

                    if (letterBelowMatches())
                        incrCounter(topToBottom, stop);

                    if (letterAboveMatches())
                        incrCounter(bottomToTop, stop);

                    if (bottomRightLetterMatches())
                        incrCounter(topToBottomRight, stop);

                    if (topRightLetterMatches())
                        incrCounter(bottomToTopRight, stop);

                    if (bottomLeftLetterMatches())
                        incrCounter(topToBottomLeft, stop);

                    if (topLeftLetterMatches())
                        incrCounter(bottomToTopLeft, stop);
                }

                if (leftToRight == word.size()) // word has been found
                {
                    while (leftToRight != 0)
                        curPositions.insert(ind + word.size() - leftToRight--);
                }

                if (rightToLeft == word.size())
                {
                    while (rightToLeft != 0)
                        curPositions.insert(ind - word.size() + rightToLeft--);
                }

                if (topToBottom == word.size())
                {
                    while (topToBottom != 0)
                    {
                        curPositions.insert(ind + lineSize * (word.size() - topToBottom) + word.size() - topToBottom);
                        topToBottom--;
                    }
                }

                if (bottomToTop == word.size())
                {
                    while (bottomToTop != 0)
                    {
                        curPositions.insert(ind - lineSize * (word.size() - bottomToTop) - word.size() + bottomToTop);
                        bottomToTop--;
                    }
                }

                if (topToBottomRight == word.size())
                {
                    while (topToBottomRight != 0)
                    {
                        curPositions.insert(ind + lineSize * (word.size() - topToBottomRight) + word.size() * 2 - topToBottomRight * 2);
                        --topToBottomRight;
                    }
                }

                if (bottomToTopRight == word.size())
                {
                    while (bottomToTopRight != 0)
                    {
                        curPositions.insert(ind - (lineSize * (word.size() - bottomToTopRight)));
                        --bottomToTopRight;
                    }
                }

                if (topToBottomLeft == word.size())
                {
                    while (topToBottomLeft != 0)
                    {
                        curPositions.insert(ind + (lineSize * (word.size() - topToBottomLeft)));
                        --topToBottomLeft;
                    }
                }

                if (bottomToTopLeft == word.size())
                {
                    while (bottomToTopLeft != 0)
                    {
                        curPositions.insert(ind - (lineSize * (word.size() - bottomToTopLeft)) - word.size() * 2 + bottomToTopLeft * 2);
                        --bottomToTopLeft;
                    }
                }
            }
        }
        positions += curPositions;
        update();
    }
}

QSize WordSearch::minimumSizeHint() const
{
    const int minWidth = lineSize * 20, minHeight = rowSize * 20 + 20;
    auto width = qMax(idealSize.width(), minWidth);
    auto height = qMax(idealSize.height(), minHeight);

    return QSize(width, height);
}

void WordSearch::paintEvent(QPaintEvent * /*event*/)
{
    if (wordSearchContents.isEmpty())
        return;
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    const auto sz = width() / (lineSize * 2);

    QFont defaultFont = painter.font();
    defaultFont.setPointSize(sz);

    QFont boldFont = defaultFont;
    boldFont.setBold(true);

    QPen defaultPen = painter.pen();
    QPen highlightText(Qt::red);

    const int startX = sz / 4, startY = sz * 1.5;
    QPoint prntPoint(startX, startY);

    painter.setFont(defaultFont);
    painter.setPen(defaultPen);

    for (QString::size_type ind = 0, row = 1; ind < wordSearchContents.size(); ++ind)
    {
        if (wordSearchContents[ind] != '\n')
        {
            if (positions.contains(ind))
            {
                painter.setFont(boldFont);
                painter.setPen(highlightText);
                painter.drawText(prntPoint, QString(wordSearchContents[ind]));
                painter.setFont(defaultFont);
                painter.setPen(defaultPen);
            }
            else
                painter.drawText(prntPoint, QString(wordSearchContents[ind]));

            prntPoint.setX(prntPoint.x() + sz * 2);
        }
        else
        {
            prntPoint.setX(startX);
            prntPoint.setY(sz * 2 * row + startY);
            ++row;
        }
    }

    idealSize = QRect(QPoint(startX, startY), prntPoint);
    idealSize.setWidth(1);
}

bool WordSearch::getText(QString &imageFile)
{
    tesseract::TessBaseAPI tess;

    QImage image(imageFile);

    if (image.isNull())
        return false;

    image = image.convertToFormat(QImage::Format_Grayscale8);

    tess.Init(NULL, "eng");
    tess.SetImage(image.bits(), image.width(), image.height(), 1, image.bytesPerLine());
    tess.SetSourceResolution(300);
    tess.DetectOS(0);

    Boxa *boxes = tess.GetComponentImages(tesseract::RIL_BLOCK, true, NULL, NULL);

    for (int i = 0; i < boxes->n; ++i)
    {
        BOX* box = boxaGetBox(boxes, i, L_CLONE);
        tess.SetRectangle(box->x, box->y, box->w, box->h);
    }
    // average confidence value is greater than 50
    if (tess.MeanTextConf() > 50)
    {
        const char *txt = tess.GetUTF8Text();
        wordSearchContents = txt;
        delete[] txt;
    }
    else
        return false;

    wordSearchContents = wordSearchContents.toUpper();
    // tesseract detects capital o as 0(zero) at times, need to replace with O(capital o)
    wordSearchContents.replace('0', 'O');
    wordSearchContents.replace(" ", "");
    wordSearchContents = wordSearchContents.trimmed();
    return true;
}

void WordSearch::findWidthHeightSize()
{
    // get amount of letters in each row(line)
    QTextStream textStream(&wordSearchContents);
    lineSize = textStream.readLine().size();

    // get number of rows/ get amount of letters in each column
    while (!textStream.readLine().isNull())
        ++rowSize;
}
