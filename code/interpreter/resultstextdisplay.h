#ifndef MINOTAUR_CPP_RESULTSTEXTDISPLAY_H
#define MINOTAUR_CPP_RESULTSTEXTDISPLAY_H

#include <QTextEdit>
#include "codeeditor.h"

class ResultsTextDisplay : public CodeEditor {
Q_OBJECT

public:
    ResultsTextDisplay(QWidget *parent = 0);
    ~ResultsTextDisplay();

private:
    void append(const QString &str);

public Q_SLOTS:
    // Slot for script submitted signal
    // append script results/errors to the display
    void appendResults(const QString &script,
                       const QString &out,
                       const QString &err);
};


#endif //MINOTAUR_CPP_RESULTSTEXTDISPLAY_H
