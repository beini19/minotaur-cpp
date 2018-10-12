#ifndef MINOTAUR_CPP_ACTIONBUTTON_H
#define MINOTAUR_CPP_ACTIONBUTTON_H

#include "../base/actionbutton.h"

#include <QPushButton>

/**
 * Push button implementation that sets a fixed size
 * and manages the button label. Used with the ActionBox.
 */
class ActionButton : public QPushButton, public base::ActionButton {
public:
    explicit ActionButton(QString &&label, QWidget *parent = nullptr);
};

#endif //MINOTAUR_CPP_ACTIONBUTTON_H
